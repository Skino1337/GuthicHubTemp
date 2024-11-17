// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GHBTTaskChangeTarget.h"


UGHBTTaskChangeTarget::UGHBTTaskChangeTarget()
{
	NodeName = "ChangeTarget";
}

EBTNodeResult::Type UGHBTTaskChangeTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// auto Grux = Cast<AGrux>(OwnerComp.GetAIOwner()->GetPawn());
	//
	// if (nullptr == Grux)
	// 	return EBTNodeResult::Failed;
	//
	//
	// Grux->BackJump();
	// IsAttacking = true;
	//
	//
	// Grux->OnAttackEnd.AddLambda([this]() -> void {
	// 	IsAttacking = false;
	//
	// });
	return EBTNodeResult::InProgress;
}