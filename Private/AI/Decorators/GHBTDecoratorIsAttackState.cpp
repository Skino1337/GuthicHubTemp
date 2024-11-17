// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/GHBTDecoratorIsAttackState.h"


UGHBTDecoratorIsAttackState::UGHBTDecoratorIsAttackState()
{
	NodeName = "Is Attack State?";
}

bool UGHBTDecoratorIsAttackState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto Npc = Cast<AGHNPC>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Npc)
		return false;

	if (Npc->State == EAIState::Attack)
		return true;
	
	return false;
}