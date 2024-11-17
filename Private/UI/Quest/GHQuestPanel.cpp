// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/GHQuestPanel.h"

#include "Game/GHQuest.h"


void UGHQuestPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	CurrentQuestButton->OnClicked.AddDynamic(this, &UGHQuestPanel::OnClickedCurrentQuestButton);
	CurrentQuestButton->OnHovered.AddDynamic(this, &UGHQuestPanel::OnHoveredCurrentQuestButton);
	CurrentQuestButton->OnUnhovered.AddDynamic(this, &UGHQuestPanel::OnUnhoveredCurrentQuestButton);

	CompletedMissionButton->OnClicked.AddDynamic(this, &UGHQuestPanel::OnClickedCompletedMissionButton);
	CompletedMissionButton->OnHovered.AddDynamic(this, &UGHQuestPanel::OnHoveredCompletedMissionButton);
	CompletedMissionButton->OnUnhovered.AddDynamic(this, &UGHQuestPanel::OnUnhoveredCompletedMissionButton);

	FailedMissionButton->OnClicked.AddDynamic(this, &UGHQuestPanel::OnClickedFailedMissionButton);
	FailedMissionButton->OnHovered.AddDynamic(this, &UGHQuestPanel::OnHoveredFailedMissionButton);
	FailedMissionButton->OnUnhovered.AddDynamic(this, &UGHQuestPanel::OnUnhoveredFailedMissionButton);

	GeneralInformationButton->OnClicked.AddDynamic(this, &UGHQuestPanel::OnClickedGeneralInformationButton);
	GeneralInformationButton->OnHovered.AddDynamic(this, &UGHQuestPanel::OnHoveredGeneralInformationButton);
	GeneralInformationButton->OnUnhovered.AddDynamic(this, &UGHQuestPanel::OnUnhoveredGeneralInformationButton);

	// FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject( this, &AMyGameMode::RespawnPlayer, Player );
	// FTimerHandle TimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, RespawnDelegate, 1, false);
}

void UGHQuestPanel::OnClickedCurrentQuestButton()
{
	if (QuestClassPointer)
		QuestClassPointer->ChangeTab(EQuestTab::Current);
}

void UGHQuestPanel::OnHoveredCurrentQuestButton()
{
	OnHovered(CurrentQuestButton);
}

void UGHQuestPanel::OnUnhoveredCurrentQuestButton()
{
	OnUnhovered(CurrentQuestButton);
}

void UGHQuestPanel::OnClickedCompletedMissionButton()
{
	if (QuestClassPointer)
		QuestClassPointer->ChangeTab(EQuestTab::Completed);
}

void UGHQuestPanel::OnHoveredCompletedMissionButton()
{
	OnHovered(CompletedMissionButton);
}

void UGHQuestPanel::OnUnhoveredCompletedMissionButton()
{
	OnUnhovered(CompletedMissionButton);
}

void UGHQuestPanel::OnClickedFailedMissionButton()
{
	if (QuestClassPointer)
		QuestClassPointer->ChangeTab(EQuestTab::Failed);
}

void UGHQuestPanel::OnHoveredFailedMissionButton()
{
	OnHovered(FailedMissionButton);
}

void UGHQuestPanel::OnUnhoveredFailedMissionButton()
{
	OnUnhovered(FailedMissionButton);
}

void UGHQuestPanel::OnClickedGeneralInformationButton()
{
	if (QuestClassPointer)
		QuestClassPointer->ChangeTab(EQuestTab::Information);
}

void UGHQuestPanel::OnHoveredGeneralInformationButton()
{
	OnHovered(GeneralInformationButton);
}

void UGHQuestPanel::OnUnhoveredGeneralInformationButton()
{
	OnUnhovered(GeneralInformationButton);
}

void UGHQuestPanel::OnHovered(UButton* Button)
{
	auto Color = FLinearColor(0.8f, 1.0f, 0.0f, 1.0f);
	auto ChildrenArray = Button->GetAllChildren();

	for (auto Children : ChildrenArray)
	{
		auto TextBlock = Cast<UTextBlock>(Children);
		if (TextBlock)
		{
			TextBlock->SetColorAndOpacity(Color);
			break;
		}
	}
}

void UGHQuestPanel::OnUnhovered(UButton* Button)
{
	auto Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	auto ChildrenArray = Button->GetAllChildren();

	for (auto Children : ChildrenArray)
	{
		auto TextBlock = Cast<UTextBlock>(Children);
		if (TextBlock)
		{
			TextBlock->SetColorAndOpacity(Color);
			break;
		}
	}
}