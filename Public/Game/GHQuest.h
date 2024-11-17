#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"



#include "UI/Quest/GHQuestPanel.h"
#include "UI/Quest/GHQuestPanelLine.h"
#include "UI/Quest/GHQuestRecords.h"

#include "GHQuest.generated.h"

// Forward Declarations
class UGHQuestPanel;
class UGHQuestPanelLine;

// enum class EQuestStatus;

UENUM()
enum EQuestTab
{
	Current,
	Completed,
	Failed,
	Information
};

UCLASS()
class GOTHICHUB_API UGHQuest : public UObject
{
	GENERATED_BODY()

public:
	UGHQuest();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> QuestPanelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHQuestPanel* QuestPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> QuestPanelLineClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> QuestRecordsClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHQuestRecords* QuestRecords;

	UFUNCTION()
	void Initialization();

	UFUNCTION()
	void ShowRecords(FQuest Quest);
	
	UFUNCTION()
	void Show();

	UFUNCTION()
	void ChangeTab(EQuestTab QuestTab);
};
