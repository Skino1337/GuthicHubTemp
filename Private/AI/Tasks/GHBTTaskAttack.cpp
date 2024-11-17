// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GHBTTaskAttack.h"

#include "Game/GHAnimInstance.h"


UGHBTTaskAttack::UGHBTTaskAttack()
{
	NodeName = "Attack";
	bNotifyTick = true;
}

EBTNodeResult::Type UGHBTTaskAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	auto Npc = Cast<AGHNPC>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Npc)
		return EBTNodeResult::Failed;

	// if (Npc->State != ENpcState::Attack)
	// 	return EBTNodeResult::Failed;

	Npc->Attack();
	// if (Montage)
	// 	UAnimInstance::Montage_Play(Montage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
	
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

void UGHBTTaskAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto Npc = Cast<AGHNPC>(OwnerComp.GetAIOwner()->GetPawn());
	if (Npc)
	{
		if (Npc->AnimInstance->bIsOnAnimate == false)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}