// ReSharper disable CppMemberFunctionMayBeStatic
#include "Game/GHDebug.h"

// in header =  circle
#include "Game/GHAnimInstance.h"


// forward declaration
#include "Game/GHGameMode.h"

#include "TimerManager.h"
#include "UI/DebugPanel/GHActionQueueLine.h"

UGHDebug::UGHDebug()
{
	// todo replace commas with dots (FCulture)
	NumberFormatOptions.AlwaysSign = false; // + and -
	NumberFormatOptions.UseGrouping = true; // 100 000 000,00
	NumberFormatOptions.RoundingMode = ERoundingMode::HalfToZero;
	NumberFormatOptions.MinimumIntegralDigits = 1;
	NumberFormatOptions.MaximumIntegralDigits = 99;
	NumberFormatOptions.MinimumFractionalDigits = 2;
	NumberFormatOptions.MaximumFractionalDigits = 2;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> DebugPanelClassFinder(TEXT("/Game/UI/DebugPanel"));
	if (DebugPanelClassFinder.Succeeded())
		DebugPanelClass = DebugPanelClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> AnimationStoryLineClassFinder(TEXT("/Game/UI/AnimationStoryLine"));
	if (AnimationStoryLineClassFinder.Class != nullptr)
		AnimationStoryLineClass = AnimationStoryLineClassFinder.Class;


	static ConstructorHelpers::FClassFinder<UUserWidget> ActionQueueLineClassFinder(TEXT("/Game/UI/ActionQueueLine"));
	if (ActionQueueLineClassFinder.Class != nullptr)
		ActionQueueLineClass = ActionQueueLineClassFinder.Class;
}

void UGHDebug::Initialization()
{
	Show();

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, this, &UGHDebug::Update, 0.016f, true, 0);

	if (DebugPanel)
	{
		if (DebugPanel->VerticalBoxAnimationStory)
			DebugPanel->VerticalBoxAnimationStory->ClearChildren();

		if (DebugPanel->VerticalBoxActionQueue)
			DebugPanel->VerticalBoxActionQueue->ClearChildren();

		if (DebugPanel->VerticalBoxActionQueueStory)
			DebugPanel->VerticalBoxActionQueueStory->ClearChildren();
	}
}

void UGHDebug::Show()
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	if (DebugPanelClass)
	{
		DebugPanel = CreateWidget<UGHDebugPanel>(World, DebugPanelClass);
		if (DebugPanel)
		{
			DebugPanel->AddToViewport();
		}
	}
}

void UGHDebug::Update()
{
	if (!DebugPanel)
		return;

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return;

	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	if (GameMode)
	{
		FString Days = FString::Printf(TEXT("Days: %d"), GameMode->GameTimeDays);
		FString Hours = FString::Printf(TEXT("%d"), GameMode->GameTimeHours);
		FString Minutes = FString::Printf(TEXT("%d"), GameMode->GameTimeMinutes);
		FString Seconds = FString::Printf(TEXT("%d"), GameMode->GameTimeSeconds);
		
		DebugPanel->TextBlockDay->SetText(FText::FromString(Days));

		if (Hours.Len() == 1)
			Hours = "0" + Hours;
		if (Minutes.Len() == 1)
			Minutes = "0" + Minutes;
		if (Seconds.Len() == 1)
			Seconds = "0" + Seconds;
		
		auto TimeText = "Time: " + Hours + ":" + Minutes + ":" + Seconds;
		DebugPanel->TextBlockTime->SetText(FText::FromString(TimeText));
	}
	
	auto Npc = Cast<AGHNPC>(UGameplayStatics::GetPlayerPawn(World, 0));
	if (!Npc)
		return;
	
	auto Position = Npc->GetActorLocation();
	auto Rotation = Npc->GetActorRotation();

	NumberFormatOptions.MinimumFractionalDigits = 2;
	NumberFormatOptions.MaximumFractionalDigits = 2;

	// Position
	
	FString PositionX = FText::AsNumber(Position.X, &NumberFormatOptions).ToString();
	PositionX = PositionX.Replace(TEXT(",") , TEXT("."));

	FString PositionY = FText::AsNumber(Position.Y, &NumberFormatOptions).ToString();
	PositionX = PositionX.Replace(TEXT(",") , TEXT("."));

	FString PositionZ = FText::AsNumber(Position.Z, &NumberFormatOptions).ToString();
	PositionX = PositionX.Replace(TEXT(",") , TEXT("."));
	
	auto PositionText = "X: " + PositionX + ", Y: " + PositionY + ", Z: " + PositionZ;

	// Rotation
	
	FString RotationRoll = FText::AsNumber(Rotation.Roll, &NumberFormatOptions).ToString();
	RotationRoll = RotationRoll.Replace(TEXT(",") , TEXT("."));

	FString RotationPitch = FText::AsNumber(Rotation.Pitch, &NumberFormatOptions).ToString();
	RotationPitch = RotationPitch.Replace(TEXT(",") , TEXT("."));

	FString RotationYaw = FText::AsNumber(Rotation.Yaw, &NumberFormatOptions).ToString();
	RotationYaw = RotationYaw.Replace(TEXT(",") , TEXT("."));

	auto RotationText = "Roll: " + RotationRoll + ", Pitch: " + RotationPitch + ", Yaw: " + RotationYaw;

	DebugPanel->TextBlockPosition->SetText(FText::FromString(PositionText));
	DebugPanel->TextBlockRotation->SetText(FText::FromString(RotationText));

	
	// auto NotifyEntry = FString::Printf(TEXT("%i) %s Class: %s Dur:%.3f"), NotifyIndex, *NotifyState.NotifyName.ToString(), *NotifyState.NotifyStateClass->GetName(), NotifyState.GetDuration());

	// Animation Story
	AnimationHistotyUpdate(Npc);

	// Action Queue
	ActionQueueUpdate(Npc);
	ActionQueueStoryUpdate(Npc);

	// Debug String
	DebugStringUpdate(Npc);
	
	// auto WidgetsLast = DebugPanel->VerticalBoxAnimationStory->GetAllChildren().Last();
	// auto AnimationStoryLineLast = Cast<UGHAnimationStoryLine>(WidgetsLast);
	// if (AnimationStoryLineLast)
	// {
	// 	auto AnimationElapsedTimeText = FText::AsNumber(Npc->AnimInstance->AnimationElapsedTime, &NumberFormatOptions);
	// 	auto AnimationElapsedTimeString = AnimationElapsedTimeText.ToString().Replace(TEXT(",") , TEXT("."));
	// 	AnimationStoryLineLast->ElapsedTime->SetText(FText::FromString(AnimationElapsedTimeString));
	// }
}

void UGHDebug::DebugStringUpdate(AGHNPC* Npc)
{
	DebugPanel->TextBlockDebugString1->SetText(FText::FromString(Npc->DebugString1));
	DebugPanel->TextBlockDebugString2->SetText(FText::FromString(Npc->DebugString2));
	DebugPanel->TextBlockDebugString3->SetText(FText::FromString(Npc->DebugString3));
	DebugPanel->TextBlockDebugString4->SetText(FText::FromString(Npc->DebugString4));
}

void UGHDebug::AnimationHistotyUpdate(AGHNPC* Npc)
{
	if (!DebugPanel || !DebugPanel->VerticalBoxAnimationStory || !AnimationStoryLineClass)
		return;

	if (!Npc || !Npc->AnimInstance)
		return;

	auto AnimationHistoryCurLen = Npc->AnimInstance->AnimationHistoryArray.Num();
	int32 AnimationWidgetMaxLen = 8;
	auto AnimationWidgetCurLen = DebugPanel->VerticalBoxAnimationStory->GetChildrenCount();
	auto NeedToShow = FMath::Min(AnimationHistoryCurLen, AnimationWidgetMaxLen);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "last name: "
	// 	+ Npc->AnimInstance->AnimationHistoryArray[AnimationHistoryCurLen - 1].Name);
	
	if (AnimationWidgetCurLen > NeedToShow)
	{
		auto NeedToDelete = AnimationWidgetCurLen - NeedToShow;
		for (int32 i = 0; i < NeedToDelete; i++)
			DebugPanel->VerticalBoxAnimationStory->RemoveChildAt(0);
	}
	else if (AnimationWidgetCurLen < NeedToShow)
	{
		auto NeedToAdd = NeedToShow - AnimationWidgetCurLen;
		for (int32 i = 0; i < NeedToAdd; i++)
		{
			auto AnimationHistoryLine = CreateWidget<UGHAnimationStoryLine>(DebugPanel->VerticalBoxAnimationStory,
				AnimationStoryLineClass);
			if (AnimationHistoryLine)
			{
				DebugPanel->VerticalBoxAnimationStory->AddChildToVerticalBox(AnimationHistoryLine);
			}
		}
	}

	AnimationHistoryCurLen = Npc->AnimInstance->AnimationHistoryArray.Num();
	AnimationWidgetCurLen = DebugPanel->VerticalBoxAnimationStory->GetChildrenCount();
	if (AnimationHistoryCurLen < AnimationWidgetCurLen)
	 	return;

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LENN == "
	// + FString::SanitizeFloat(AnimationWidgetCurLen));
	// if (AnimationWidgetCurLen == 1)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LENN == "
	// 		+ Npc->AnimInstance->AnimationHistoryArray[0].Name);
	// }
	for (int32 i = 1; i <= AnimationWidgetCurLen; i++)
	{
		auto Widget = DebugPanel->VerticalBoxAnimationStory->GetChildAt(AnimationWidgetCurLen - i);
		auto AnimationHistoryLine = Cast<UGHAnimationStoryLine>(Widget);
		if (!AnimationHistoryLine)
			continue;

		auto bIsEmptyName = Npc->AnimInstance->AnimationHistoryArray[AnimationHistoryCurLen - i].Name.IsEmpty();

		// Name
		auto Name = FText();
		if (bIsEmptyName)
			Name = FText::FromString("No Animation");
		else 
			Name = FText::FromString(Npc->AnimInstance->AnimationHistoryArray[AnimationHistoryCurLen - i].Name);
		AnimationHistoryLine->AnimationName->SetText(Name);

		// RootMotion
		auto RootMotionText = FText::FromString("");
		if (!bIsEmptyName)
		{
			RootMotionText = Npc->AnimInstance->AnimationHistoryArray[AnimationHistoryCurLen - i].bRootMotion
				? FText::FromString("RM") : FText::FromString("");
		}
		AnimationHistoryLine->RootMotion->SetText(RootMotionText);

		// // Duration
		// if (Npc->AnimInstance->AnimationHistoryArray[i].Duration <= 0.002f)
		// {
		// 	AnimationHistoryLine->ElapsedTime->SetText(FText::FromString(""));
		// }
		// else
		// {
		NumberFormatOptions.MinimumFractionalDigits = 3;
		NumberFormatOptions.MaximumFractionalDigits = 3;
		
		auto DurationString = FText::AsNumber(Npc->AnimInstance->AnimationHistoryArray
			[AnimationHistoryCurLen - i].Duration, &NumberFormatOptions).ToString();
		DurationString = DurationString.Replace(TEXT(",") , TEXT("."));
		AnimationHistoryLine->ElapsedTime->SetText(FText::FromString(DurationString));
		// }

		// Interrupted
		auto InterruptedText = FText::FromString("");
		if (i != 1)
		{
			if (!bIsEmptyName)
			{
				InterruptedText = Npc->AnimInstance->AnimationHistoryArray[AnimationHistoryCurLen - i].bInterrupted
					? FText::FromString("True") : FText::FromString("False");
			}
		}
		
		AnimationHistoryLine->Interrupted->SetText(InterruptedText);
	}
}

void UGHDebug::AnimationElapsedUpdate(float Elapsed)
{
	// if (!DebugPanel || !DebugPanel->VerticalBoxAnimationStory || !AnimationStoryLineClass)
	// 	return;
	//
	// auto AnimationStoryCurLen = DebugPanel->VerticalBoxAnimationStory->GetChildrenCount();
	// if (AnimationStoryCurLen > 0)
	// {
	// 	auto AnimationStoryLineLast = Cast<UGHAnimationStoryLine>(DebugPanel->VerticalBoxAnimationStory->GetAllChildren().Last());
	// 	if (AnimationStoryLineLast)
	// 	{
	// 		NumberFormatOptions.MinimumFractionalDigits = 3;
	// 		NumberFormatOptions.MaximumFractionalDigits = 3;
	// 		
	// 		auto ElapsedText = FText::AsNumber(Elapsed, &NumberFormatOptions).ToString();
	// 		ElapsedText = ElapsedText.Replace(TEXT(",") , TEXT("."));
	// 		
	// 		AnimationStoryLineLast->ElapsedTime->SetText(FText::FromString(ElapsedText));
	// 	}
	// }
}

void UGHDebug::AnimationStartUpdate(const FString& AnimationName)
{
	// if (!DebugPanel || !DebugPanel->VerticalBoxAnimationStory || !AnimationStoryLineClass)
	// 	return;
	//
	// auto AnimationStoryMaxLen = 8;
	// auto AnimationStoryCurLen = DebugPanel->VerticalBoxAnimationStory->GetChildrenCount();
	// if (AnimationStoryCurLen >= AnimationStoryMaxLen && AnimationStoryCurLen > 0)
	// 	DebugPanel->VerticalBoxAnimationStory->RemoveChildAt(0);
	//
	// auto AnimationStoryLine = CreateWidget<UGHAnimationStoryLine>(DebugPanel->VerticalBoxAnimationStory,
	// 	AnimationStoryLineClass);
	// if (AnimationStoryLine)
	// {
	// 	AnimationStoryLine->AnimationName->SetText(FText::FromString(AnimationName));
	// 	AnimationStoryLine->ElapsedTime->SetText(FText::FromString(""));
	// 	AnimationStoryLine->Interrupted->SetText(FText::FromString(""));
	//
	// 	DebugPanel->VerticalBoxAnimationStory->AddChildToVerticalBox(AnimationStoryLine);
	// }
}

void UGHDebug::AnimationEndUpdate(float Elapsed, bool bInterrupted)
{
	// if (!DebugPanel || !DebugPanel->VerticalBoxAnimationStory || !AnimationStoryLineClass)
	// 	return;
	//
	// auto AnimationStoryCurLen = DebugPanel->VerticalBoxAnimationStory->GetChildrenCount();
	// if (AnimationStoryCurLen > 0)
	// {
	// 	auto AnimationStoryLineLast = Cast<UGHAnimationStoryLine>(DebugPanel->VerticalBoxAnimationStory->GetAllChildren().Last());
	// 	if (AnimationStoryLineLast)
	// 	{
	// 		NumberFormatOptions.MinimumFractionalDigits = 3;
	// 		NumberFormatOptions.MaximumFractionalDigits = 3;
	// 		
	// 		auto ElapsedText = FText::AsNumber(Elapsed, &NumberFormatOptions).ToString();
	// 		ElapsedText = ElapsedText.Replace(TEXT(",") , TEXT("."));
	// 		auto InterruptedText = bInterrupted ? FText::FromString("True") : FText::FromString("False");
	// 		if (Elapsed == -1.0f)
	// 			InterruptedText = FText::FromString("");
	// 		else
	// 			AnimationStoryLineLast->ElapsedTime->SetText(FText::FromString(ElapsedText));
	//
	// 		AnimationStoryLineLast->Interrupted->SetText(InterruptedText);
	// 	}
	// }
}

void UGHDebug::ActionQueueStoryUpdate(AGHNPC* NPC)
{
	if (!DebugPanel || !DebugPanel->VerticalBoxActionQueueStory || !ActionQueueLineClass)
		return;
	
	if (!NPC || !NPC->ActionQueue)
		return;

	auto ActionQueueStoryCurLen = NPC->ActionQueue->ActionQueueStory.Num();
	int32 ActionQueueStoryWidgetMaxLen = 3;
	auto ActionQueueStoryWidgetCurLen = DebugPanel->VerticalBoxActionQueueStory->GetChildrenCount();
	auto NeedToShow = FMath::Min(ActionQueueStoryCurLen, ActionQueueStoryWidgetMaxLen);
	
	if (ActionQueueStoryCurLen == 0)
	{
		NeedToShow = 1;
	}

	if (ActionQueueStoryWidgetCurLen > NeedToShow)
	{
		auto NeedToDelete = ActionQueueStoryWidgetCurLen - NeedToShow;
		for (int32 i = 0; i < NeedToDelete; i++)
			DebugPanel->VerticalBoxActionQueueStory->RemoveChildAt(0);
	}
	else if (ActionQueueStoryWidgetCurLen < NeedToShow)
	{
		auto NeedToAdd = NeedToShow - ActionQueueStoryWidgetCurLen;
		for (int32 i = 0; i < NeedToAdd; i++)
		{
			auto ActionQueueHistoryLine = CreateWidget<UGHActionQueueLine>(DebugPanel->VerticalBoxActionQueueStory,
				ActionQueueLineClass);
			if (ActionQueueHistoryLine)
			{
				auto WidgetTransform = FWidgetTransform();
				WidgetTransform.Scale = FVector2d(1.0f, -1.0f);
				ActionQueueHistoryLine->SetRenderTransform(WidgetTransform);
				
				DebugPanel->VerticalBoxActionQueueStory->AddChildToVerticalBox(ActionQueueHistoryLine);
			}
		}
	}
	
	if (ActionQueueStoryCurLen == 0)
	{
		auto Widget = DebugPanel->VerticalBoxActionQueueStory->GetChildAt(0);
		auto ActionQueueLine = Cast<UGHActionQueueLine>(Widget);
		if (!ActionQueueLine)
			return;

		ActionQueueLine->ActionTypeName->SetText(FText::FromString(TEXT("NO ACTIONS")));
		ActionQueueLine->ActionInformation->SetText(FText::FromString(TEXT("")));

		return;
	}

	// !          THISSSSSSSSSSS!!!!!!!!!!!!!!!!!
	ActionQueueStoryWidgetCurLen = DebugPanel->VerticalBoxActionQueueStory->GetChildrenCount();
	int32 IndexWindget = -1;
	for (int32 i = ActionQueueStoryCurLen - 1; i >= 0; i--)
	{
		IndexWindget++;
		
		if (IndexWindget >= ActionQueueStoryWidgetCurLen)
			continue;
		
		auto Widget = DebugPanel->VerticalBoxActionQueueStory->GetChildAt(IndexWindget);
		auto ActionQueueLine = Cast<UGHActionQueueLine>(Widget);
		if (!ActionQueueLine)
			continue;

		auto MessageType = NPC->ActionQueue->ActionQueueStory[i]->MessageType;
		auto Name = UEnum::GetValueAsString(MessageType);
		Name.RemoveAt(0, 13, EAllowShrinking::No);
	
		auto MessageInformation = NPC->ActionQueue->ActionQueueStory[i]->MessageInformation;
		
		ActionQueueLine->ActionTypeName->SetText(FText::FromString(Name));
		ActionQueueLine->ActionInformation->SetText(FText::FromString(MessageInformation));
	}

	// // !          THISSSSSSSSSSS!!!!!!!!!!!!!!!!!
	// ActionQueueStoryWidgetCurLen = DebugPanel->VerticalBoxActionQueueStory->GetChildrenCount(); // 3
	// for (int32 i = 0; i <= ActionQueueStoryWidgetCurLen; i++)
	// {
	// 	auto IndexReverse = ActionQueueStoryWidgetCurLen - (i + 1); // 3
	// 	auto Widget = DebugPanel->VerticalBoxActionQueueStory->GetChildAt(i);
	// 	auto ActionQueueLine = Cast<UGHActionQueueLine>(Widget);
	// 	if (!ActionQueueLine)
	// 		continue;
	// 	
	// 	if (IndexReverse >= NPC->ActionQueue->ActionQueueStory.Num()) // 3 >= 4
	// 		continue;
	// 	
	// 	auto MessageType = NPC->ActionQueue->ActionQueueStory[IndexReverse]->MessageType;
	// 	auto Name = UEnum::GetValueAsString(MessageType);
	// 	Name.RemoveAt(0, 13, EAllowShrinking::No);
	//
	// 	auto MessageInformation = NPC->ActionQueue->ActionQueueStory[IndexReverse]->MessageInformation;
	// 	
	// 	ActionQueueLine->ActionTypeName->SetText(FText::FromString(Name));
	// 	ActionQueueLine->ActionInformation->SetText(FText::FromString(MessageInformation));
	// }
}

void UGHDebug::ActionQueueUpdate(AGHNPC* NPC)
{
	if (!DebugPanel || !DebugPanel->VerticalBoxActionQueue || !ActionQueueLineClass)
		return;
	
	if (!NPC || !NPC->ActionQueue)
		return;

	auto ActionQueueCurLen = NPC->ActionQueue->ActionQueue.Num();
	int32 ActionWidgetMaxLen = 3;
	auto ActionWidgetCurLen = DebugPanel->VerticalBoxActionQueue->GetChildrenCount();
	auto NeedToShow = FMath::Min(ActionQueueCurLen, ActionWidgetMaxLen);

	if (ActionQueueCurLen == 0)
	{
		NeedToShow = 1;
	}

	if (ActionWidgetCurLen > NeedToShow)
	{
		auto NeedToDelete = ActionWidgetCurLen - NeedToShow;
		for (int32 i = 0; i < NeedToDelete; i++)
			DebugPanel->VerticalBoxActionQueue->RemoveChildAt(0);
	}
	else if (ActionWidgetCurLen < NeedToShow)
	{
		auto NeedToAdd = NeedToShow - ActionWidgetCurLen;
		for (int32 i = 0; i < NeedToAdd; i++)
		{
			auto ActionQueueLine = CreateWidget<UGHActionQueueLine>(DebugPanel->VerticalBoxActionQueue,
				ActionQueueLineClass);
			if (ActionQueueLine)
			{
				DebugPanel->VerticalBoxActionQueue->AddChildToVerticalBox(ActionQueueLine);
			}
		}
	}
	
	if (ActionQueueCurLen == 0)
	{
		auto Widget = DebugPanel->VerticalBoxActionQueue->GetChildAt(0);
		auto ActionQueueLine = Cast<UGHActionQueueLine>(Widget);
		if (!ActionQueueLine)
			return;

		ActionQueueLine->ActionTypeName->SetText(FText::FromString(TEXT("NO ACTIONS")));
		ActionQueueLine->ActionInformation->SetText(FText::FromString(TEXT("")));

		return;
	}

	ActionWidgetCurLen = DebugPanel->VerticalBoxActionQueue->GetChildrenCount();
	for (int32 i = 0; i <= ActionWidgetCurLen; i++)
	{
		auto Widget = DebugPanel->VerticalBoxActionQueue->GetChildAt(i);
		auto ActionQueueLine = Cast<UGHActionQueueLine>(Widget);
		if (!ActionQueueLine)
			continue;
		
		if (i >= NPC->ActionQueue->ActionQueue.Num())
			continue;
		
		auto MessageType = NPC->ActionQueue->ActionQueue[i]->MessageType;
		auto Name = UEnum::GetValueAsString(MessageType);
		Name.RemoveAt(0, 13, EAllowShrinking::No);

		auto MessageInformation = NPC->ActionQueue->ActionQueue[i]->MessageInformation;
		
		ActionQueueLine->ActionTypeName->SetText(FText::FromString(Name));
		ActionQueueLine->ActionInformation->SetText(FText::FromString(MessageInformation));
	}
}