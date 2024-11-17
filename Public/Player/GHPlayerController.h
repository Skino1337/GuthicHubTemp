// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Components/WidgetComponent.h"

#include "Core/Helpers.h"
#include "Game/GHNPC.h"

#include "GHPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API AGHPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGHPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	//TSoftObjectPtr<UInputMappingContext> GothicInputMappingContext;
	class UInputMappingContext* GothicInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DialogAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* StatsAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* QuestAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* PossesAction;

	//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RotIncAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RotDecAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ChangeAxisAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* TurnLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* TurnRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MeleeWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RangeWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MagicWeaponAction;
	
	//
	
	UFUNCTION() // Called for looking input
	void Look(const FInputActionValue& Value);

	UFUNCTION() // Called for movement input */
	void Move(const FInputActionValue& Value);

	UFUNCTION() // Called for jumping input */
	void Jump(const FInputActionValue& Value);

	UFUNCTION() // Called for jumping input */
	void StopJumping(const FInputActionValue& Value);
	
	UFUNCTION() // Called for open inventory
	void Inventory(const FInputActionValue& Value);

	UFUNCTION() // Called for start dialog
	void Dialog(const FInputActionValue& Value);

	UFUNCTION() // Called for open stats
	void Stats(const FInputActionValue& Value);

	UFUNCTION() // Called for open quests
	void Quest(const FInputActionValue& Value);

	UFUNCTION() // Called for posses on npc
	void Posses(const FInputActionValue& Value);

	// Rot
	UFUNCTION() // Called for posses on npc
	void RotInc(const FInputActionValue& Value);

	UFUNCTION() // Called for posses on npc
	void RotDec(const FInputActionValue& Value);

	UFUNCTION() // Called for posses on npc
	void ChangeAxisCF(const FInputActionValue& Value);
	
	UFUNCTION()
	void StartMoveForward(const FInputActionValue& Value);

	UFUNCTION()
	void StopMoveForward(const FInputActionValue& Value);

	UFUNCTION()
	void StartMoveBackward(const FInputActionValue& Value);

	UFUNCTION()
	void StopMoveBackward(const FInputActionValue& Value);

	UFUNCTION()
	void StartMoveLeft(const FInputActionValue& Value);

	UFUNCTION()
	void StopMoveLeft(const FInputActionValue& Value);

	UFUNCTION()
	void StartMoveRight(const FInputActionValue& Value);

	UFUNCTION()
	void StopMoveRight(const FInputActionValue& Value);

	UFUNCTION()
	void StartTurnLeft(const FInputActionValue& Value);

	UFUNCTION()
	void StopTurnLeft(const FInputActionValue& Value);

	UFUNCTION()
	void StartTurnRight(const FInputActionValue& Value);

	UFUNCTION()
	void StopTurnRight(const FInputActionValue& Value);
	
	UFUNCTION()
	void MeleeWeaponToggle(const FInputActionValue& Value);

	UFUNCTION()
	void RangeWeaponToggle(const FInputActionValue& Value);

	UFUNCTION()
	void MagicWeaponToggle(const FInputActionValue& Value);
};
