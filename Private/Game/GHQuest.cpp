#include "Game/GHQuest.h"
#include "Game/GHGameMode.h"


UGHQuest::UGHQuest()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> QuestPanelClassFinder(TEXT("/Game/UI/QuestPanel"));
	if (QuestPanelClassFinder.Class != nullptr)
		QuestPanelClass = QuestPanelClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> QuestPanelLineClassFinder(TEXT("/Game/UI/QuestPanelLine"));
	if (QuestPanelLineClassFinder.Class != nullptr)
		QuestPanelLineClass = QuestPanelLineClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> QuestRecordsClassFinder(TEXT("/Game/UI/QuestRecords"));
	if (QuestRecordsClassFinder.Class != nullptr)
		QuestRecordsClass = QuestRecordsClassFinder.Class;
}

void UGHQuest::Initialization()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHQuest::Initialization");

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	if (QuestPanelClass)
	{
		QuestPanel = CreateWidget<UGHQuestPanel>(World, QuestPanelClass);
		QuestPanel->QuestClassPointer = this;
	}

	if (QuestRecordsClass && QuestPanel)
	{
		QuestRecords = CreateWidget<UGHQuestRecords>(QuestPanel, QuestRecordsClass);
	}
}

void UGHQuest::ShowRecords(FQuest Quest)
{
	if (QuestPanel)
	{
		if (QuestPanel->IsInViewport())
		{
			QuestPanel->RemoveFromParent();
		}
	}

	if (QuestRecords && QuestPanelLineClass)
	{
		auto World = GEngine->GetCurrentPlayWorld(nullptr);
		auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

		QuestRecords->VerticalBox->ClearChildren();
		for (auto Record : Quest.Records)
		{
			auto TextLine = CreateWidget<UGHQuestPanelLine>(QuestRecords, QuestPanelLineClass);
			if (TextLine)
			{
				auto TranslatedString = GameMode->GetTranslatedString(Record);
				TextLine->TextBlock->SetText(FText::FromString(TranslatedString));
				QuestRecords->VerticalBox->AddChildToVerticalBox(TextLine);
			}
		}

		QuestRecords->AddToViewport();
	}
}

void UGHQuest::Show()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHQuest::Show");

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	if (QuestPanel)
	{
		if (QuestPanel->IsInViewport())
		{
			QuestPanel->RemoveFromParent();
			QuestPanel->VerticalBox->ClearChildren();
			return;
		}
		
		ChangeTab(EQuestTab::Current);
		
		QuestPanel->AddToViewport();

		auto PlayerController = UGameplayStatics::GetPlayerController(World, 0);
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
	
		// Log_CreateTopic(CH1_JoinOC,LOG_MISSION);
  //   	Log_SetTopicStatus(CH1_JoinOC,LOG_RUNNING);
  //   	B_LogEntry(CH1_JoinOC,"Чтобы присоединиться к людям Гомеза в Старом Лагере, мне нужно увидеться с Торусом.");
}

void UGHQuest::ChangeTab(EQuestTab CurrentTab)
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	if (QuestPanel && QuestPanelLineClass)
	{
		QuestPanel->VerticalBox->ClearChildren();
		
		for (auto Quest : GameMode->QuestMap)
		{
			if (CurrentTab == EQuestTab::Current)
			{
				if (Quest.Value.Status == EQuestStatus::Running)
				{
					auto TextLine = CreateWidget<UGHQuestPanelLine>(QuestPanel, QuestPanelLineClass);
					if (TextLine)
					{
						auto TranslatedString = GameMode->GetTranslatedString(Quest.Value.Name);
						
						TextLine->QuestClassPointer = this;
						TextLine->QuestStruct = Quest.Value;
						TextLine->TextBlock->SetText(FText::FromString(TranslatedString));
						QuestPanel->VerticalBox->AddChildToVerticalBox(TextLine);
					}
				}
			}
			else if (CurrentTab == EQuestTab::Completed)
			{
				if (Quest.Value.Status == EQuestStatus::Success)
				{
					auto TextLine = CreateWidget<UGHQuestPanelLine>(QuestPanel, QuestPanelLineClass);
					if (TextLine)
					{
						auto TranslatedString = GameMode->GetTranslatedString(Quest.Value.Name);
						
						TextLine->QuestClassPointer = this;
						TextLine->QuestStruct = Quest.Value;
						TextLine->TextBlock->SetText(FText::FromString(TranslatedString));
						QuestPanel->VerticalBox->AddChildToVerticalBox(TextLine);
					}
				}
			}
			else if (CurrentTab == EQuestTab::Failed)
			{
				if (Quest.Value.Status == EQuestStatus::Failed)
				{
					auto TextLine = CreateWidget<UGHQuestPanelLine>(QuestPanel, QuestPanelLineClass);
					if (TextLine)
					{
						auto TranslatedString = GameMode->GetTranslatedString(Quest.Value.Name);
						
						TextLine->QuestClassPointer = this;
						TextLine->QuestStruct = Quest.Value;
						TextLine->TextBlock->SetText(FText::FromString(TranslatedString));
						QuestPanel->VerticalBox->AddChildToVerticalBox(TextLine);
					}
				}
			}
		}
	}
}