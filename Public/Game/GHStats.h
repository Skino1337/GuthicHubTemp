// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "UI/GHStatsPanel.h"
#include "UI/GHStatsLine.h"

#include "GHStats.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API UGHStats : public UObject
{
	GENERATED_BODY()

public:
	UGHStats();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> StatsPanelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> StatsLineClass;
	
	void Initialization();

	UFUNCTION()
	void Show(AGHNPC* Npc);
};
