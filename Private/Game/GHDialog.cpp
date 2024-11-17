// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHDialog.h"
#include "UI/GHDialogChoicesButton.h"

#include "Components/AudioComponent.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/ThreadManager.h"
#include "AudioDevice.h"
#include "Game/GHGameMode.h"
#include "ShaderPrintParameters.h"

UGHDialog::UGHDialog()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> DialogTextClassFinder(TEXT("/Game/UI/DialogText"));
	if (DialogTextClassFinder.Class != nullptr)
		DialogTextClass = DialogTextClassFinder.Class;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> DialogChoicesClassFinder(TEXT("/Game/UI/DialogChoices"));
	if (DialogChoicesClassFinder.Class != nullptr)
		DialogChoicesClass = DialogChoicesClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> DialogChoicesButtonClassFinder(TEXT("/Game/UI/DialogChoicesButton"));
	if (DialogChoicesButtonClassFinder.Class != nullptr)
		DialogChoicesButtonClass = DialogChoicesButtonClassFinder.Class;
}

void UGHDialog::Initialization()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHDialog::Initialization");
	
	// delegete bugs, dont change
	World = GEngine->GetCurrentPlayWorld(nullptr);
	
	if (DialogTextClass)
		DialogText = CreateWidget<UGHDialogText>(World, DialogTextClass);

	if (DialogChoicesClass)
		DialogChoices = CreateWidget<UGHDialogChoices>(World, DialogChoicesClass);

	// if (DialogChoicesButtonClass)
	// 	DialogChoicesButton = CreateWidget<UGHDialogChoicesButton>(World, DialogChoicesButtonClass);

	RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	if (IsValid(RuntimeAudioImporter))
	{
		RuntimeAudioImporter->OnResultNative.AddWeakLambda(this, [this](URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status)
		{
			if (Status == ERuntimeImportStatus::SuccessfulImport)
			{
				UAudioComponent* Audio = UGameplayStatics::CreateSound2D(World, ImportedSoundWave);
				if (IsValid(Audio))
				{
					Audio->VolumeMultiplier = 0.05f;
					Audio->bAutoDestroy = true;
					Audio->Play();
					
					float Duration = ImportedSoundWave->Duration * 1.10f;
					FTimerHandle TimerHandleWidget;
					World->GetTimerManager().SetTimer(TimerHandleWidget, this, &UGHDialog::EndWidgetDialogText, Duration, false);
				}
				else
				{
					// TODO: Correct duration
					float Duration = 2.0f;
					FTimerHandle TimerHandleWidget;
					World->GetTimerManager().SetTimer(TimerHandleWidget, this, &UGHDialog::EndWidgetDialogText, Duration, false);
					UE_LOG(LogTemp, Error, TEXT("if (IsValid(Audio))"));
				}
			}
			else
			{
				// TODO: Correct duration
				float Duration = 2.0f;
				FTimerHandle TimerHandleWidget;
				World->GetTimerManager().SetTimer(TimerHandleWidget, this, &UGHDialog::EndWidgetDialogText, Duration, false);
				UE_LOG(LogTemp, Error, TEXT("Failed to import audio"));
			}
		});
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create audio importer"));
	}
}

void UGHDialog::Interact(AGHCharacter* Player, AGHNPC* Npc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Interact");
	
	if (Npc->_table.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGHDialog::Start Npc->_table.Type != ELuaValueType::Table"));
		return;
	}

	PlayerLatest = Player;
	NpcLatest = Npc;
	auto PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	LuaDialogThread = FLuaValue();
	for (auto Dialog : Npc->Dialogs)
	{
		if (Dialog.Important)
		{
			if (!Dialog.Permanent && DialogKnowInfoArray.Contains(Dialog.Name))
				continue;
			
			bool bIsConditionSuccess = false;
			if (Dialog.Condition.Type == ELuaValueType::Function)
			{
				FLuaValue ConditionReturnValue = ULuaBlueprintFunctionLibrary::LuaValueCall(Dialog.Condition, {});
				if (ConditionReturnValue.Type == ELuaValueType::Bool)
					bIsConditionSuccess = ConditionReturnValue.ToBool();
			}
			else if (Dialog.Condition.Type == ELuaValueType::Bool)
			{
				bIsConditionSuccess = Dialog.Condition.ToBool();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cond err type"));
			}
	
			if (bIsConditionSuccess)
			{
				DialogStart(Dialog);
				return;
			}
		}
	}

	if (DialogChoices && DialogChoicesButtonClass)
	{
		TArray<FDialog> ValidDialogArray;
		for (auto Dialog : Npc->Dialogs)
		{
			if (Dialog.Important)
				continue;

			if (!Dialog.Permanent && DialogKnowInfoArray.Contains(Dialog.Name))
				continue;

			bool bIsConditionSuccess = false;
			if (Dialog.Condition.Type == ELuaValueType::Function)
			{
				FLuaValue ConditionReturnValue = ULuaBlueprintFunctionLibrary::LuaValueCall(Dialog.Condition, {});
				if (ConditionReturnValue.Type == ELuaValueType::Bool)
					bIsConditionSuccess = ConditionReturnValue.ToBool();
			}
			else if (Dialog.Condition.Type == ELuaValueType::Bool)
			{
				bIsConditionSuccess = Dialog.Condition.ToBool();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cond err type"));
			}
			if (!bIsConditionSuccess)
				continue;

			if (Dialog.Information.Type != ELuaValueType::Function)
				continue;
			
			ValidDialogArray.Add(Dialog);
		}

		DialogChoices->DialogChoicesBox->ClearChildren();
		for (auto Dialog : ValidDialogArray)
		{
			DialogChoicesButton = CreateWidget<UGHDialogChoicesButton>(DialogChoices, DialogChoicesButtonClass);
			if (DialogChoicesButton)
			{
				auto TranslatedString = GameMode->GetTranslatedString(Dialog.Description);
				
				DialogChoicesButton->DialogClassPointer = this;
				DialogChoicesButton->DialogStruct = Dialog;
				DialogChoicesButton->DialogChoicesButtonText->SetText(FText::FromString(TranslatedString));
				DialogChoices->DialogChoicesBox->AddChildToVerticalBox(DialogChoicesButton);
			}
		}
		
		if (DialogChoices->DialogChoicesBox->GetChildrenCount() > 0)
		{
			DialogChoices->AddToViewport();
			
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;
			PlayerController->bEnableClickEvents = true;
			PlayerController->bEnableMouseOverEvents = true;

			return;
		}
	}

	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;
}

void UGHDialog::DelayedPlayAudio(FString AudioPath)
{
	if (IsValid(RuntimeAudioImporter))
		RuntimeAudioImporter->ImportAudioFromFile(AudioPath, ERuntimeAudioFormat::Auto);
}

void UGHDialog::DialogStart(FDialog Dialog)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHDialog::DialogStart");

	if (IsValid(World))
	{
		if (Dialog.Information.Type == ELuaValueType::Function)
		{
			LuaDialogThread = ULuaBlueprintFunctionLibrary::LuaCreateThread(World, UGHLuaState::StaticClass(), Dialog.Information);
			if (LuaDialogThread.Type == ELuaValueType::Thread)
			{
				ULuaBlueprintFunctionLibrary::LuaGlobalCallMulti(World, UGHLuaState::StaticClass(), "coroutine.resume", {LuaDialogThread});
				if (!Dialog.Permanent)
					DialogKnowInfoArray.Add(Dialog.Name);

				auto PlayerController = UGameplayStatics::GetPlayerController(World, 0);
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->bShowMouseCursor = false;
				PlayerController->bEnableClickEvents = false;
				PlayerController->bEnableMouseOverEvents = false;

				if (DialogChoices)
					DialogChoices->RemoveFromParent();
			}
		}
	}
}

void UGHDialog::CreateWidgetDialogText(FLuaValue Sources, FLuaValue Destination, FLuaValue Text)
{
	if (DialogText)
	{
		auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
		FString TranslatedString = Text.ToString();
		TranslatedString = GameMode->GetTranslatedString(TranslatedString);
		
		DialogText->UpdateText(TranslatedString);
		DialogText->AddToViewport();
	}
	
	if (IsValid(RuntimeAudioImporter))
	{
		auto Dial = Text.ToString().ToUpper();
		
		TArray<FString> DataArray;
		auto PartsNum = Dial.ParseIntoArray(DataArray,TEXT("."),true);
	
		FString AudioFileName = "";
		if (DataArray.Num() > 0)
			AudioFileName = DataArray.Last() + ".WAV";
		
		FString AudioPath = "C:/gothic_lua_scripts/_test_001/audio/" + AudioFileName;
		DelayedPlayAudio(AudioPath);
	}

	// FTimerHandle TimerHandleWidget;
	// World->GetTimerManager().SetTimer(TimerHandleWidget, this, &UGHDialog::EndWidgetDialogText, 2.0f, false);
	
	if (LuaDialogThread.Type == ELuaValueType::Thread)
	{
		auto CoroutineStatusReturnValueArray = ULuaBlueprintFunctionLibrary::LuaGlobalCallMulti(World,
			UGHLuaState::StaticClass(),"coroutine.status", {LuaDialogThread});
		if (CoroutineStatusReturnValueArray.Num() > 0)
		{
			auto CoroutineStatus = CoroutineStatusReturnValueArray[0].ToString();
			if (CoroutineStatus == "normal")
			{
				ULuaBlueprintFunctionLibrary::LuaValueYield(LuaDialogThread,{});
			}
		}
	}
}

void UGHDialog::EndWidgetDialogText()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHDialog::EndWidgetDialogText");
	
	if (DialogText)
		DialogText->RemoveFromParent();
	
	if (LuaDialogThread.Type == ELuaValueType::Thread)
	{
		ULuaBlueprintFunctionLibrary::LuaGlobalCallMulti(World, UGHLuaState::StaticClass(),
			"coroutine.resume", {LuaDialogThread});
		auto CoroutineStatusReturnValueArray = ULuaBlueprintFunctionLibrary::LuaGlobalCallMulti(World,
			UGHLuaState::StaticClass(),"coroutine.status", {LuaDialogThread});
		if (CoroutineStatusReturnValueArray.Num() > 0)
		{
			auto CoroutineStatus = CoroutineStatusReturnValueArray[0].ToString();
			if (CoroutineStatus == "suspended")
			{
				// ???
			}
			else if (CoroutineStatus == "dead")
			{
				Interact(PlayerLatest, NpcLatest);
			}
		}
	}
}