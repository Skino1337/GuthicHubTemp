#include "UI/Quest/GHQuestPanelLine.h"

#include "Game/GHQuest.h"

void UGHQuestPanelLine::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button->OnClicked.AddDynamic(this, &UGHQuestPanelLine::OnClicked);
	Button->OnHovered.AddDynamic(this, &UGHQuestPanelLine::OnHovered);
	Button->OnUnhovered.AddDynamic(this, &UGHQuestPanelLine::OnUnhovered);
}

void UGHQuestPanelLine::OnClicked()
{
	if (QuestClassPointer)
		QuestClassPointer->ShowRecords(QuestStruct);
}

void UGHQuestPanelLine::OnHovered()
{
	auto Color = FLinearColor(0.8f, 1.0f, 0.0f, 1.0f);
	TextBlock->SetColorAndOpacity(Color);
}

void UGHQuestPanelLine::OnUnhovered()
{
	auto Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	TextBlock->SetColorAndOpacity(Color);
}