#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GHQuestStruct.generated.h"

UENUM()
enum class EQuestType : uint8
{
	Mission = 0,
	Note = 1
};

UENUM()
enum class EQuestStatus : uint8
{
	Running = 1,
	Success = 2,
	Failed = 3,
	Obsolete = 4
};

USTRUCT(BlueprintType)
struct FQuest
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EQuestType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EQuestStatus Status;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FString> Records;

	FQuest()
	{
		Name = "";
		Type = EQuestType::Mission;
		Status = EQuestStatus::Running;
		Records = {};
	}
};
