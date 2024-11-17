// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GHBTTaskChaseTarget.h"

#include "Core/Helpers.h"
#include "Navigation/PathFollowingComponent.h"

UGHBTTaskChaseTarget::UGHBTTaskChaseTarget()
{
	NodeName = "Chase Target";
	bNotifyTick = true;
}

EBTNodeResult::Type UGHBTTaskChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	auto Npc = Cast<AGHNPC>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Npc)
		return EBTNodeResult::Failed;
	
	auto LuaValueTarget = Npc->GetLuaValue("target");
	if (LuaValueTarget.Type != ELuaValueType::UObject)
		return EBTNodeResult::Failed;

	auto Target = Helpers::GetNpcFromLuaValue(LuaValueTarget);
	if (!Target)
		return EBTNodeResult::Failed;

	auto AIController = Cast<AGHAIController>(Npc->GetController());
	if (AIController)
	{
		// AIController->bIsMoving = true;
		AIController->MoveToActor(Target, 150);
		//AIController->OnMoveCompleted()
		//DECLARE_MULTICAST_DELEGATE_TwoParams(FMoveCompletedSignature, FAIRequestID /*RequestID*/, EPathFollowingResult::Type /*Result*/);
		//DECLARE_MULTICAST_DELEGATE_TwoParams(FMoveComplete, FAIRequestID /*RequestID*/, const FPathFollowingResult& /*Result*/);
		//AIController->ReceiveMoveCompleted.AddDynamic(Npc, &UGHBTTaskChaseTarget::OnMoveEnd);
	}
	
	// auto PathStart = Npc->GetActorLocation();
	// auto PathEnd = Target->GetActorLocation();
	//
	// // https://supervj.top/2020/05/27/AI_MoveTo%E7%A0%94%E7%A9%B6/
	// auto NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, PathStart, PathEnd);
	// if (NavigationPath && NavigationPath->IsValid())
	// {
	// 	FAIMoveRequest req;
	// 	req.SetAcceptanceRadius(150);
	// 	req.SetUsePathfinding(true);
	// 	
	// 	auto AIController = Cast<AAIController>(Npc->GetController());
	// 	if (AIController)
	// 	{
	// 		//RequestID = AIController->RequestMove(req, NavigationPath->GetPath());
	// 		AIController->MoveToActor(Target);
	// 	}
	// }
	
	return EBTNodeResult::InProgress;
}

// void UGHBTTaskChaseTarget::OnMoveEnd(FAIRequestID RequestID, EPathFollowingResult::Type Result)
// {
// 	bIsMoving = false;
// }

void UGHBTTaskChaseTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto Npc = Cast<AGHNPC>(OwnerComp.GetAIOwner()->GetPawn());
	if (Npc)
	{
		auto AIController = Cast<AGHAIController>(Npc->GetController());
		if (AIController)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "FinishLatentTask");
			
			AIController->bIsMoving = false;
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}

	// if (RequestID.IsValid())
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "VALID!!!!!");
	// }
	// else
	// {
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "NOT VALID!!!!!");
	// }
	// auto Npc = Cast<AGHNPC>(OwnerComp.GetAIOwner()->GetPawn());
	// if (Npc)
	// {
	// 	if (Npc->bIsOnAnimAction == false)
	// 	{
	// 		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// 	}
	// }
}