// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "Game/GHNPC.h"
#include "AIController.h"

#include "GHBTTaskAttack.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API UGHBTTaskAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UGHBTTaskAttack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
