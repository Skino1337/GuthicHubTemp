// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"

#include "Game/GHNPC.h"
//#include "Game/GHGameMode.h"

#include "UI/GHInventorySlots.h"
#include "UI/GHInventoryItem.h"

#include "GHInventory.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API UGHInventory : public UObject
{
	GENERATED_BODY()

public:
	UGHInventory();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHInventorySlots* InventorySlots;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> InventorySlotsClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> InventoryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UGHInventoryItem*> InventoryItemArray;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns = 5;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows = 6;

	void Initialization();

	UFUNCTION()
	void ShowInventory(AGHNPC* Npc);
};
