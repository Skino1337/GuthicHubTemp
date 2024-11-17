// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GHAIController.h"



AGHAIController::AGHAIController()
{
	// static ConstructorHelpers::FClassFinder<UBehaviorTree> BehaviorTreeClassFinder(TEXT("/Game/AI/BehaviorTreeNPC"));
	// if (BehaviorTreeClassFinder.Succeeded())
	// 	BehaviorTreeClass = BehaviorTreeClassFinder.Class;
	
	// static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/AI/BT_Grux.BT_Grux'"));
 //    if (BTObject.Succeeded())
 //    {
 //    	BTGrux = BTObject.Object;
 //    }
		
	bIsMoving = false;
}

void AGHAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Npc = Cast<AGHNPC>(InPawn);
	if (!Npc)
		return;
	
	auto InstanceName = Npc->_table.GetField("name").ToString();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AIController OnPossess: " + InstanceName);
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return;
	
	FTimerHandle MovementUpdateTimer;
	World->GetTimerManager().SetTimer(MovementUpdateTimer, this, &AGHAIController::MovementUpdate, 0.200f, true);
	
	// ABot* Bot = Cast<ABot>(InPawn);
	//
	// // start behavior
	// if (Bot && Bot->BotBehavior)
	// {
	// 	BlackboardComp->InitializeBlackboard(Bot->BotBehavior->BlackboardAsset);
	//
	// 	LaserKeyID = BlackboardComp->GetKeyID("Laser");
	// 	
	//
	// 	BehaviorComp->StartTree(Bot->BotBehavior);
	// }
}

void AGHAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OnMoveCompleted cpp");

	auto WaypointArrayLen = Npc->MoveWaypointArray.Num();
	if (WaypointArrayLen <= 0)
		return;
	
	if (Result.IsSuccess())
	{
		Npc->MoveWaypointArray.RemoveAt(WaypointArrayLen - 1);
		
		// go to next wp if he exist
		if (WaypointArrayLen >= 2)
			StartWaypointMovement();
	}
}

void AGHAIController::MovementUpdate()
{
	auto WaypointArrayLen = Npc->MoveWaypointArray.Num();
	if (WaypointArrayLen <= 0)
		return;
	
	if (PathFollowingRequestResult == EPathFollowingRequestResult::Type::Failed)
	{
		// on very fast start (init), or point not reachable
		auto GenerationRadius = Npc->NavigationInvoker->GetGenerationRadius();
		Npc->NavigationInvoker->SetGenerationRadii(GenerationRadius * 1.20f, 6000.0f);
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "PathFindingUpdate FAIL "
		// 	+ Npc->MoveWaypointArray[WaypointArrayLen - 1].Name);

		// one more time try loop
		StartWaypointMovement();
	}
	else
	{
		Npc->NavigationInvoker->SetGenerationRadii(2000.0f, 6000.0f);
	}
	// else if (PathFollowingRequestResult == EPathFollowingRequestResult::Type::AlreadyAtGoal)
	// {
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "PathFindingUpdate AlreadyAtGoal "
	// 	// 	+ Npc->MoveWaypointArray[WaypointArrayLen - 1].Name);
	// 	//
	// 	// Npc->NavigationInvoker->SetGenerationRadii(2000.0f, 6000.0f);
	// 	// Npc->MoveWaypointArray.RemoveAt(WaypointArrayLen - 1);
	// 	//
	// 	// // go to next wp if he exist
	// 	// if (WaypointArrayLen >= 2)
	// 	// 	StartWaypointMovement();
	// }
	// else if (PathFollowingRequestResult == EPathFollowingRequestResult::Type::RequestSuccessful)
	// {
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "PathFindingUpdate RequestSuccessful"
	// 	// 	+ Npc->MoveWaypointArray[WaypointArrayLen - 1].Name);
	// }
}

void AGHAIController::StartWaypointMovement()
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "StartWaypointMovement 0");
	
	auto WaypointArrayLen = Npc->MoveWaypointArray.Num();
	if (WaypointArrayLen <= 0)
		return;
	
	auto NextWaypoint = Npc->MoveWaypointArray[WaypointArrayLen - 1];
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "StartWaypointMovement 1 " + NextWaypoint.Name);
	
	// finish move if pawn gets close enough
	auto AcceptanceRadius = 20.0f;
	
	// add pawn's radius to AcceptanceRadius
	auto bStopOnOverlap = false;

	// use navigation data to calculate path (otherwise it will go in straight line)
	auto bUsePathfinding = true;

	// project location on navigation data before using it
	auto bProjectDestinationToNavigation = false;

	// set focus related flag: bAllowStrafe
	auto bCanStrafe = false;

	// navigation filter for pathfinding adjustments. If none specified DefaultNavigationFilterClass will be used
	// FilterClass

	// use incomplete path when goal can't be reached
	auto bAllowPartialPath = false;
	
	PathFollowingRequestResult = MoveToLocation(NextWaypoint.Position, AcceptanceRadius, bStopOnOverlap,
		bUsePathfinding, bProjectDestinationToNavigation);
}