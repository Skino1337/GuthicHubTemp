// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Navigation/PathFollowingComponent.h"

#include "Game/GHNPC.h"
#include "AI/GHAIController.h"

#include "GHBTTaskChaseTarget.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API UGHBTTaskChaseTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UGHBTTaskChaseTarget();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// UFUNCTION()
	// void OnMoveEnd(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
