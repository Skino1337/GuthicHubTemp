// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GHBTTaskChangeTarget.generated.h"


UCLASS()
class GOTHICHUB_API UGHBTTaskChangeTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGHBTTaskChangeTarget();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

// protected:
// 	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSecondes) override;
};
