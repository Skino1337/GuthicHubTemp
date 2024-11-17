// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// ---

#include "Struct/GHEventMessageStruct.h"

// ---

#include "GHNPCActionQueue.generated.h"


// Forward declaration
class AGHNPC;


// https://www.reddit.com/r/unrealengine/comments/15g9qqt/multiple_inheritance_in_unreal_engine_c/
// https://dev.epicgames.com/documentation/en-us/unreal-engine/interfaces?application_version=4.27


UCLASS()
class GOTHICHUB_API UGHNPCActionQueue: public UObject
{
	GENERATED_BODY()

public:
	UGHNPCActionQueue();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGHNPC* NPC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEventMessage*> ActionQueueStory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEventMessage*> ActionQueue;
	
	// ---
	// Action: Weapon
	// ---
		
	UFUNCTION()
	void DrawWeaponMeleeActionAdd();
		
	UFUNCTION()
	void DrawWeaponMeleeActionExecute();
		
	UFUNCTION()
	void DrawWeaponMeleeActionEnd();

	// ---
	
	UFUNCTION()
	void DrawWeaponRangeActionAdd();

	UFUNCTION()
	void DrawWeaponRangeActionExecute();

	UFUNCTION()
	void DrawWeaponRangeActionEnd();

	// ---
		
	UFUNCTION()
	void RemoveWeaponMeleeActionAdd();
		
	UFUNCTION()
	void RemoveWeaponMeleeActionExecute();
	
	UFUNCTION()
	void RemoveWeaponMeleeActionEnd();

	// ---
	
	UFUNCTION()
	void RemoveWeaponRangeActionAdd();
	
	UFUNCTION()
	void RemoveWeaponRangeActionExecute();

	UFUNCTION()
	void RemoveWeaponRangeActionEnd();
	
	// ---
	
	UFUNCTION()
	void WeaponActionCheck(UWeaponMessage* WeaponMessage);
	
	// ---
	// Action: Weapon
	// ---
	
	// ---
	// Action: UseItem
	// ---

	UFUNCTION()
	void EquipItemActionAdd(const FString& ItemInstance);

	UFUNCTION()
	void EquipItemActionExecute(const FString& ItemInstanceName);

	UFUNCTION()
	void EquipItemActionEnd();

	UFUNCTION()
	void UseItemActionCheck(UUseItemMessage* UseItemMessage);

	// ----
	// Action: UseItem
	// ----

	UFUNCTION()
	void RemoveActionFromQueue();

	UFUNCTION()
	void ExecuteActionQueue();
};
