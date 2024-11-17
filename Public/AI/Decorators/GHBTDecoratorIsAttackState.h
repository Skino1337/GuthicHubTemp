// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTDecorator.h"

#include "Game/GHNPC.h"
#include "AIController.h"

#include "GHBTDecoratorIsAttackState.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API UGHBTDecoratorIsAttackState : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UGHBTDecoratorIsAttackState();
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
