// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

// #include "BehaviorTree/BlackboardComponent.h"
// #include "BehaviorTree/BehaviorTreeComponent.h"
// #include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
// #include "BehaviorTree/BlackboardComponent.h"
// #include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
// #include "BehaviorTree/BTTaskNode.h"
// #include "BehaviorTree/BlackboardComponent.h"
// #include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
// #include "BehaviorTree/BehaviorTree.h"

#include "Game/GHNPC.h"

#include "GHAIController.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API AGHAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGHAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGHNPC* Npc;

	// use old style.. https://forums.unrealengine.com/t/structs-enums-and-bp-editor/73747/8
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EPathFollowingRequestResult::Type> PathFollowingRequestResult;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoving;

	UFUNCTION()
	void MovementUpdate();

	UFUNCTION()
	void StartWaypointMovement();
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TSubclassOf<UBehaviorTree> BehaviorTreeClass;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UBehaviorTreeComponent* BTreeComp;
	// UPROPERTY()
	// TSubobjectPtr<class UBehaviorTreeComponent> BehaviorComp;
};
