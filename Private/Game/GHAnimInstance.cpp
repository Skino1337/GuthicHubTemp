// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHAnimInstance.h"

#include "glTFRuntimeAsset.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "VectorUtil.h"
#include "Core/GHAssetManager.h"


#include "Core/GHGLTFRuntime.h"
#include "Game/GHAnimNotify.h"

#include "Kismet/GameplayStatics.h"

#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/CheatManager.h"

DEFINE_LOG_CATEGORY(SkAnimation);

// https://zhuanlan.zhihu.com/p/622301809
UGHAnimInstance::UGHAnimInstance()
{
	// bIsMoveForwardPressed = false;
	// bIsMoveBackwardPressed = false;
	// IdleAnimationName = "S_RUN";
	// MoveForwardAnimationName = "S_RUNL";
	// MoveBackwardAnimationName = "T_JUMPB";
	// MoveLeftAnimationName = "T_RUNSTRAFEL";
	// MoveRightAnimationName = "T_RUNSTRAFER";
	
	IdleAnimationData = nullptr;
	MoveForwardAnimationData = nullptr;
	MoveBackwardAnimationData = nullptr;
	MoveLeftAnimationData = nullptr;
	MoveRightAnimationData = nullptr;
	TurnLeftAnimationData = nullptr;
	TurnRightAnimationData = nullptr;

	BodyState = EBodyState::None;

	// PreviousAnimationData = FAnimationData();
	// CurrentAnimationData = FAnimationData();

	AnimationHistoryArray = {FAnimationHistory()};
	// AnimationHistoryArray.Add(FAnimationHistory());
	// auto AnimationHistory = FAnimationHistory();
	// AnimationHistory.Name = "No Animation";
	// AnimationHistory.Duration = 0.0f;
	// AnimationHistoryArray.Add(AnimationHistory);
	
	// bIsOnAnimate = false;
	bIsRunStateMachine = false;
	// CurrentMontageName = "";
}

void UGHAnimInstance::SetupSkeletonSlots(USkeleton* Skeleton)
{
	if (!Skeleton)
		return;

	// @TODO: add all slots, mb all
	TArray<FString> SlotArray = {"1", "2", "4", "9", "11", "20"};
	
	for (auto Slot : SlotArray)
	{
		FString GroupName = "G" + Slot;
		Skeleton->AddSlotGroupName(FName(GroupName));
		if (Skeleton->FindAnimSlotGroup(FName(GroupName)))
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton have Group: " + GroupName);

			FString SlotName = "S" + Slot;
			Skeleton->SetSlotGroupName(FName(SlotName), FName(GroupName));
			if (Skeleton->ContainsSlotName(FName(SlotName)))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton have Slot: " + SlotName);
			}
			else
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton slot NOT FOUND: " + SlotName);
			}
		}
	}
}

void UGHAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(this));
	
	//NotifyQueue.AddAnimNotify(const FAnimNotifyEvent* Notify, const UObject* NotifySource);
	OnMontageBlendingOut.AddUniqueDynamic(this, &ThisClass::OnMontageBlendingOutStarted);
	OnMontageStarted.AddUniqueDynamic(this, &ThisClass::FunctionToExecuteOnAnimationStarted);
	OnMontageEnded.AddUniqueDynamic(this, &ThisClass::FunctionToExecuteOnAnimationEnd);

	// fill to [] op open, else crush
	if (CurrentSkeleton)
	{
		auto SlotGroupArray = CurrentSkeleton->GetSlotGroups();
		for (auto SlotGroup : SlotGroupArray)
		{
			for (auto SlotName : SlotGroup.SlotNames)
			{
				auto SlotNameString = SlotName.ToString();
				if (SlotNameString.Len() <= 0)
					continue;

				FString LayerName = "";
				if (SlotNameString.Mid(0, 1) == "S")
					LayerName = SlotNameString.Mid(1, SlotNameString.Len() - 1);
				else
					LayerName = SlotNameString.Mid(0, SlotNameString.Len());

				if (LayerName.IsEmpty())
					continue;
				
				PreviousAnimationDataLayerMap.Add(LayerName, nullptr);
				CurrentAnimationDataLayerMap.Add(LayerName, nullptr);
				
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton have Slot: " + LayerName);
			}
		}
	}

	// hardcode, if some was wrong
	TArray<FString> SlotArray = {"1", "2", "4", "9", "11", "20"};
	
	if (PreviousAnimationDataLayerMap.Num() <= 0)
	{
		for (auto Slot : SlotArray)
		{
			PreviousAnimationDataLayerMap.Add(Slot, nullptr);
		}
	}

	if (CurrentAnimationDataLayerMap.Num() <= 0)
	{
		for (auto Slot : SlotArray)
		{
			CurrentAnimationDataLayerMap.Add(Slot, nullptr);
		}
	}

	// 63845789696,0 ???
	AnimationElapsedTime = 0.0f;

	// for time to "No Animation"
	AnimationStartTime = FDateTime::Now();

	Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Pawn OK");
		Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Character OK");
			CharacterMovementComponent = Character->GetCharacterMovement();
			SkeletalMeshComponent = Character->GetMesh();
			if (SkeletalMeshComponent)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SkeletalMeshComponent OK");
				SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
			}
		}

		Npc = Cast<AGHNPC>(Pawn);
		if (Npc)
		{
			// 
		}
	}

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (World)
	{
		FTimerHandle DelayedBeginPlayTimer;
		World->GetTimerManager().SetTimer(DelayedBeginPlayTimer, this, &UGHAnimInstance::DelayedInit, 0.1f, false);
	}
}

void UGHAnimInstance::SetupBlendSpaces()
{
	auto BowAimOffsetBlendSpaceTemp = NewObject<UAimOffsetBlendSpace>(this , UAimOffsetBlendSpace::StaticClass());

	// setup anim sequence
	AnimSequenceBowAimUp = GetAnimSequenceByName("C_BOW_3");
	if (!AnimSequenceBowAimUp)
	{
		UE_LOG(SkAnimation, Warning, TEXT("Can't find anim sequence with name C_BOW_3"));
		return;
	}
	
	AnimSequenceBowAimForward = GetAnimSequenceByName("C_BOW_6");
	if (!AnimSequenceBowAimForward)
	{
		UE_LOG(SkAnimation, Warning, TEXT("Can't find anim sequence with name C_BOW_6"));
		return;
	}
	
	AnimSequenceBowAimDown = GetAnimSequenceByName("C_BOW_9");
	if (!AnimSequenceBowAimDown)
	{
		UE_LOG(SkAnimation, Warning, TEXT("Can't find anim sequence with name C_BOW_9"));
		return;
	}

	// setup additive
	AnimSequenceBowAimUp->AdditiveAnimType = EAdditiveAnimationType::AAT_RotationOffsetMeshSpace;
	AnimSequenceBowAimUp->RefPoseType = EAdditiveBasePoseType::ABPT_RefPose;

	AnimSequenceBowAimForward->AdditiveAnimType = EAdditiveAnimationType::AAT_RotationOffsetMeshSpace;
	AnimSequenceBowAimForward->RefPoseType = EAdditiveBasePoseType::ABPT_RefPose;

	AnimSequenceBowAimDown->AdditiveAnimType = EAdditiveAnimationType::AAT_RotationOffsetMeshSpace;
	AnimSequenceBowAimDown->RefPoseType = EAdditiveBasePoseType::ABPT_RefPose;

	// rebuild anim data
	AnimSequenceBowAimUp->ClearAllCachedCookedPlatformData();
	AnimSequenceBowAimUp->BeginCacheDerivedDataForCurrentPlatform();

	AnimSequenceBowAimForward->ClearAllCachedCookedPlatformData();
	AnimSequenceBowAimForward->BeginCacheDerivedDataForCurrentPlatform();

	AnimSequenceBowAimDown->ClearAllCachedCookedPlatformData();
	AnimSequenceBowAimDown->BeginCacheDerivedDataForCurrentPlatform();

	// AnimSequenceBowAimUp->ValidateModel();
	// if (AnimSequenceBowAimUp->IsDataModelValid() && AnimSequenceBowAimUp->DataModelInterface->GetNumBoneTracks() && bNeedPostProcess)
	// {		
	// 	AnimSequenceBowAimUp->ClearAllCachedCookedPlatformData();
	// 	AnimSequenceBowAimUp->BeginCacheDerivedDataForCurrentPlatform();
	// }
	
	//
	// AnimSequenceBowAimForward->AdditiveAnimType = EAdditiveAnimationType::AAT_RotationOffsetMeshSpace;
	// AnimSequenceBowAimForward->RefPoseType = EAdditiveBasePoseType::ABPT_RefPose;
	//
	// AnimSequenceBowAimDown->AdditiveAnimType = EAdditiveAnimationType::AAT_RotationOffsetMeshSpace;
	// AnimSequenceBowAimDown->RefPoseType = EAdditiveBasePoseType::ABPT_RefPose;


		

	// auto SamplesNum = BowAimOffsetBlendSpaceTemp->GetNumberOfBlendSamples();
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SamplesNum after add Up: " + FString::FromInt(SamplesNum));

	BowAimOffsetBlendSpaceTemp->SetSkeleton(AnimSequenceBowAimUp->GetSkeleton());
	if (AnimSequenceBowAimUp)
	{
		auto AddIndex = BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimUp, FVector(0.0f, 90.0f, 0.0f));
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Up add index: " + FString::FromInt(AddIndex));
	}
	
	BowAimOffsetBlendSpaceTemp->SetSkeleton(AnimSequenceBowAimForward->GetSkeleton());
	if (AnimSequenceBowAimForward)
	{
		auto AddIndex = BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimForward, FVector(0.0f, 0.0f, 0.0f));
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Forward add index: " + FString::FromInt(AddIndex));
	}

	BowAimOffsetBlendSpaceTemp->SetSkeleton(AnimSequenceBowAimDown->GetSkeleton());
	if (AnimSequenceBowAimDown)
	{
		auto AddIndex = BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimDown, FVector(0.0f, -90.0f, 0.0f));
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Down add index: " + FString::FromInt(AddIndex));
	}

	// SamplesNum = BowAimOffsetBlendSpaceTemp->GetNumberOfBlendSamples();
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SamplesNum after add Forward: " + FString::FromInt(SamplesNum));

	// auto SamplesNum = BowAimOffsetBlendSpaceTemp->GetNumberOfBlendSamples();
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SamplesNum after add: " + FString::FromInt(SamplesNum));
	
	// blend param automatically expanded (if min = -90, expand to -100 <=> 100)
	// if (AnimSequenceBowAimUp && AnimSequenceBowAimForward && AnimSequenceBowAimDown)
	// {
	// 	BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimUp, FVector(0.0f, 90.0f, 0.0f));
	// 	BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimForward, FVector(0.0f, 0.0f, 0.0f));
	// 	BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimDown, FVector(0.0f, -90.0f, 0.0f));
	// }
	
	// if (AnimSequenceBowAimUpLeft && AnimSequenceBowAimForwardLeft && AnimSequenceBowAimDownLeft)
	// {
	// 	BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimUpLeft, FVector(-40.0f, 90.0f, 0.0f));
	// 	BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimForwardLeft, FVector(-40.0f, 0.0f, 0.0f));
	// 	BowAimOffsetBlendSpaceTemp->AddSample(AnimSequenceBowAimDownLeft, FVector(-40.0f, -90.0f, 0.0f));
	// }
	
	auto SamplesNum = BowAimOffsetBlendSpaceTemp->GetNumberOfBlendSamples();
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SamplesNum after add: " + FString::FromInt(SamplesNum));
	
	BowAimOffsetBlendSpaceTemp->ResampleData();
	BowAimOffsetBlendSpaceTemp->ValidateSampleData();
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SamplesNum: after Resample & Validate" + FString::FromInt(SamplesNum));
	
	auto BlendParameter = BowAimOffsetBlendSpaceTemp->GetBlendParameter(0);
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "BlendParameter (x) Min: " + FString::FromInt(BlendParameter.Min));
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "BlendParameter (x) Max: " + FString::FromInt(BlendParameter.Max));
	
	BlendParameter = BowAimOffsetBlendSpaceTemp->GetBlendParameter(1);
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "BlendParameter (y) Min: " + FString::FromInt(BlendParameter.Min));
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "BlendParameter (y) Max: " + FString::FromInt(BlendParameter.Max));

	BowAimOffsetBlendSpace = BowAimOffsetBlendSpaceTemp;
}

void UGHAnimInstance::DelayedInit()
{
	// 
	// AnimMontageIdle = GetMontageByName(IdleAnimationName);
	// AnimMontageMoveForward = GetMontageByName(MoveForwardAnimationName);
}

void UGHAnimInstance::UpdateBodyState()
{
	// check for key is valid
	auto AnimationDataMainLayer = CurrentAnimationDataLayerMap.FindRef("1");
	if (!AnimationDataMainLayer)
		return;
	
	// @TODO: add all body states
	if (AnimationDataMainLayer->Name == "S_RUN")
	{
		BodyState = EBodyState::Stand;
	}
	else if (AnimationDataMainLayer->Name == "S_WALKL")
	{
		BodyState = EBodyState::Walk;
	}
	else if (AnimationDataMainLayer->Name == "T_RUN_2_RUNL"
		|| AnimationDataMainLayer->Name == "S_RUNL"
		|| AnimationDataMainLayer->Name == "T_RUNL_2_RUN"
		|| AnimationDataMainLayer->Name == "T_RUNR_2_RUN")
	{
		BodyState = EBodyState::Run;
	}
	else
	{
		BodyState = EBodyState::Stand;
	}
}

void UGHAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (CharacterMovementComponent)
	{
		GroundSpeed = CharacterMovementComponent->Velocity.Size2D();
		bShouldMove = GroundSpeed > 3 && CharacterMovementComponent->GetCurrentAcceleration() != FVector::ZeroVector;
		bIsFalling = CharacterMovementComponent->IsFalling();
	}

	UpdateBodyState();

	// GetPosition

	// TODO: Montage layers
	AnimationElapsedTime = (FDateTime::Now() - AnimationStartTime).GetTotalSeconds();

	// for (int32 InstanceIndex = 0; InstanceIndex < MontageInstances.Num(); InstanceIndex++)
	// {
	// 	const FAnimMontageInstance* MontageInstance = MontageInstances[InstanceIndex];
	// 	if (MontageInstance && MontageInstance->IsActive())
	// 	{
	// 		return MontageInstance->GetPosition();
	// 	}
	// }

	// Events
	for (auto MontageInstance : MontageInstances)
	{
		if (!MontageInstance || !MontageInstance->IsActive())
			continue;

		for (auto CurrentAnimationDataTuple : CurrentAnimationDataLayerMap)
		{
			// @TODO: refactoring taked bot for key -> for value
			auto CurrentAnimationData = CurrentAnimationDataLayerMap.FindRef(CurrentAnimationDataTuple.Key);
			if (!CurrentAnimationData)
				continue;
			
			if (CurrentAnimationData->AnimMontage == MontageInstance->Montage
				|| CurrentAnimationData->AnimMontageNoRootMotion == MontageInstance->Montage)
			{
				for (int32 i = 0; i < CurrentAnimationData->Events.Num(); i++)
				{
					// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, CurrentAnimationData->Events[i].Name);
					// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, CurrentAnimationData->Events[i].Name);

					// reverve realize when animsequence create
					auto MontagePosition = MontageInstance->GetPosition();
					if (CurrentAnimationData->Events[i].Time >= 0 &&
						MontagePosition > CurrentAnimationData->Events[i].Time)
					{
						OnEventTrigged(CurrentAnimationData->Events[i]);
						CurrentAnimationData->Events[i].Time = -1.0f;
					}
				}
			}
		}
	}
	
	// auto CurrMontagePosition = Montage_GetPosition(nullptr);
	// //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::SanitizeFloat(CurrMontagePosition));
	//
	// for (int32 i = 0; i < CurrentAnimationDataLayerMap["1"].Events.Num(); i++)
	// {
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, CurrentAnimationData.Events[i].Name);
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, CurrentAnimationData.Events[i].Name);
	// 	
	// 	if (CurrMontagePosition > CurrentAnimationDataLayerMap["1"].Events[i].Time)
	// 	{
	// 		// TODO: check reverse work
	// 		OnEventTrigged(CurrentAnimationDataLayerMap["1"].Events[i]);
	// 		CurrentAnimationDataLayerMap["1"].Events[i].Time = 99999999.0f; // infinity float
	// 	}
	// }

	
	// if (bIsRunStateMachine)
	// 	UpdateStateMachine(DeltaSeconds);

	AnimationHistoryUpdate(DeltaSeconds);

	// Debug String
	if (Npc)
	{
		auto AnimationDataLayer1 = CurrentAnimationDataLayerMap.FindRef("1");
		if (AnimationDataLayer1)
		{
			Npc->DebugString2 = "AnimationDataLayer1: " + AnimationDataLayer1->Name;
		}
		else
		{
			Npc->DebugString2 = "AnimationDataLayer1: nullptr";
		}

		// auto AnimationDataLayer20 = CurrentAnimationDataLayerMap.FindRef("20");
		// if (AnimationDataLayer20)
		// {
		// 	Npc->DebugString3 = "AnimationDataLayer20: " + AnimationDataLayer20->Name;
		// }
		// else
		// {
		// 	Npc->DebugString3 = "AnimationDataLayer20: nullptr";
		// }
		
		auto AnimationDataLayer20 = CurrentAnimationDataLayerMap.FindRef("20");
		if (AnimationDataLayer20)
		{
			Npc->DebugString4 = "AnimationDataLayer20: " + AnimationDataLayer20->Name;
		}
		else
		{
			Npc->DebugString4 = "AnimationDataLayer20: nullptr";
		}
	}

	//FString Msg = "CurrentMontagePosition: " + FString::SanitizeFloat(CurrentMontagePosition, 3);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Msg);
	
	// auto bAnimMontageIdleText = IsValid(AnimMontageIdle) ? FText::FromString("True") : FText::FromString("False");
	// auto bShouldMoveText = bShouldMove ? FText::FromString("True") : FText::FromString("False");
	// FString Msg = "ShouldMoveText: " + bShouldMoveText.ToString();
	// Msg += ", AnimMontageIdle valid : " + bAnimMontageIdleText.ToString();
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Msg);
	
	// if (PreviousAnimationData == CurrentAnimationData)
	// {
	// 	if (CurrentAnimationData.IsValid())
	// 	{
	// 		// auto CurrentPlayableMontage = GetCurrentActiveMontage(CurrentAnimationData ? RM ???);
	// 		// null = current montage (first of one)
	// 		// todo add RM check
	// 		CurrentMontagePosition = Montage_GetPosition(nullptr);
	// 		// zero check work?
	// 		if (PreviousMontagePosition > CurrentMontagePosition && CurrentMontagePosition != 0.0f)
	// 			OnMontageChangeSection();
	//
	// 		PreviousMontagePosition = CurrentMontagePosition;
	// 	}
	// }
	// else
	// {
	// 	PreviousMontage = CurrentMontage;
	// 	PreviousMontagePosition = 0.0f;
	// 	CurrentMontagePosition = Montage_GetPosition(CurrentMontage);
	// }

	if (!Npc)
		return;

	if (!CurrentAnimationDataLayerMap["1"])
		return;
	
	auto CurrentCameraZoom = Npc->CameraBoom->TargetArmLength;
	if (CurrentCameraZoom > Npc->CameraZoomMinValue || CurrentCameraZoom < Npc->CameraZoomMaxValue)
	{
		auto CameraDistanceTotal = CurrentCameraZoom;

		if (CurrentAnimationDataLayerMap["1"] == MoveForwardAnimationData || CurrentAnimationDataLayerMap["1"]->Name.Contains("RUNL"))
		{
			if (bIsMoveForwardPressed)
			{
				CameraDistanceTotal = CameraDistanceTotal + (Npc->CameraZoomOutDistanceForTime * DeltaSeconds);
			}
			else
			{
				CameraDistanceTotal = CameraDistanceTotal - (Npc->CameraZoomInDistanceForTime * DeltaSeconds);
			}
		}
		else
		{
			CameraDistanceTotal = CameraDistanceTotal - (Npc->CameraZoomInDistanceForTime * DeltaSeconds);
		}

		CameraDistanceTotal = FMath::Clamp(CameraDistanceTotal, Npc->CameraZoomMinValue, Npc->CameraZoomMaxValue);
		Npc->CameraBoom->TargetArmLength = CameraDistanceTotal;
	}

	// Npc->IsPlayerControlled()
	auto PlayerController = Cast<APlayerController>(Npc->Controller);
	if (!PlayerController)
		return;

	auto ControlRotation = PlayerController->GetControlRotation();
	auto ActorRotation = Npc->GetActorRotation();
	auto DeltaRotation = ControlRotation - ActorRotation;
	DeltaRotation.Normalize();

	BowAimPitch = DeltaRotation.Pitch;
	BowAimYaw = DeltaRotation.Yaw;
	
	// TODO: check move left/right + rotation for body roll
	if (CurrentAnimationDataLayerMap["1"] == MoveForwardAnimationData
		|| CurrentAnimationDataLayerMap["1"] == MoveLeftAnimationData || CurrentAnimationDataLayerMap["1"] == MoveRightAnimationData
		|| CurrentAnimationDataLayerMap["1"]->Name.Contains("RUNL") || CurrentAnimationDataLayerMap["1"]->Name.Contains("WALKL"))
	{
		// counter block, not like original... (orig left prior)
		if (bIsTurnLeftPressed && !bIsTurnRightPressed)
		{
			if (Npc->WalkMode == EWalkMode::Run)
				ActorRotation.Roll = ActorRotation.Roll - (Npc->RollTurnDegreesForTime * DeltaSeconds);
			PlayerController->AddYawInput((Npc->YawTurnDegreesForTime * DeltaSeconds) * -1.0f);
		}
		else if (bIsTurnRightPressed && !bIsTurnLeftPressed)
		{
			if (Npc->WalkMode == EWalkMode::Run)
				ActorRotation.Roll = ActorRotation.Roll + (Npc->RollTurnDegreesForTime * DeltaSeconds);
			PlayerController->AddYawInput(Npc->YawTurnDegreesForTime * DeltaSeconds);
		}
		else
		{
			if (ActorRotation.Roll > 0.0f)
			{
				ActorRotation.Roll = ActorRotation.Roll - (Npc->RollTurnDegreesForTime * DeltaSeconds);
				ActorRotation.Roll = FMath::Clamp(ActorRotation.Roll, 0.0f, Npc->YawTurnSpeed);
			}
			else
			{
				ActorRotation.Roll = ActorRotation.Roll + (Npc->RollTurnDegreesForTime * DeltaSeconds);
				ActorRotation.Roll = FMath::Clamp(ActorRotation.Roll, -Npc->YawTurnSpeed, 0.0f);
			}
		}

		ActorRotation.Roll = FMath::Clamp(ActorRotation.Roll, -Npc->YawTurnSpeed, Npc->YawTurnSpeed);

		Npc->SetActorRotation(ActorRotation);
	}
	else
	{
		if (ActorRotation.Roll > 0.0f)
		{
			ActorRotation.Roll = ActorRotation.Roll - (Npc->RollTurnDegreesForTime * DeltaSeconds);
			ActorRotation.Roll = FMath::Clamp(ActorRotation.Roll, 0.0f, Npc->RollTurnSpeed);
		}
		else
		{
			ActorRotation.Roll = ActorRotation.Roll + (Npc->RollTurnDegreesForTime * DeltaSeconds);
			ActorRotation.Roll = FMath::Clamp(ActorRotation.Roll, -Npc->RollTurnSpeed, 0.0f);
		}

		ActorRotation.Roll = FMath::Clamp(ActorRotation.Roll, -Npc->RollTurnSpeed, Npc->RollTurnSpeed);

		Npc->SetActorRotation(ActorRotation);
	}
	
	if (CurrentAnimationDataLayerMap["1"] == IdleAnimationData || CurrentAnimationDataLayerMap["1"] == MoveBackwardAnimationData)
	{
		// counter block
		if (bIsTurnLeftPressed && !bIsTurnRightPressed)
		{
			PlayerController->AddYawInput((Npc->YawTurnDegreesForTime * DeltaSeconds) * -1.0f);
		}
		else if (bIsTurnRightPressed && !bIsTurnLeftPressed)
		{
			PlayerController->AddYawInput(Npc->YawTurnDegreesForTime * DeltaSeconds);
		}
	}
}

/** Called when a montage has started */
void UGHAnimInstance::FunctionToExecuteOnAnimationStarted(UAnimMontage* Montage)
{
	// AnimationStartTime = FDateTime::Now();
	// UE_LOG(SkAnimation, Warning, TEXT("FunctionToExecuteOnAnimationStarted"));
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
	// 	"AnimationStarted: " + Montage->GetName()); // CurrentAnimationName
	//
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	// if (GameMode)
	// 	GameMode->Debug->AnimationStartUpdate(Montage->GetName());
	
	// bIsOnAnimate = true;
	// CurrentMontageName = Montage->GetName();
}

/** Called when a montage has ended, whether interrupted or finished */
void UGHAnimInstance::FunctionToExecuteOnAnimationEnd(UAnimMontage* Montage, bool bInterrupted)
{
	// auto AnimationElapsedTime = AnimationStartTime - FDateTime::Now();
	// auto AnimationElapsedSeconds = AnimationElapsedTime.GetTotalSeconds();
	// FString Msg = "OnAnimationEnd, name: " + Montage->GetName()
	// 	+ ", elapsed: " + FString::SanitizeFloat(AnimationElapsedSeconds, 3)
	// 	+ ", interrupted: " + FString(bInterrupted ? "true" : "false");
	// UE_LOG(SkAnimation, Warning, TEXT("%s"), *Msg);
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Msg);
	
	// bIsOnAnimate = false;
	// CurrentMontageName = "";

	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	// if (!GameInstance)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!GameInstance"));
	// 	return;
	// }
	//
	// if (!Npc)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!Npc"));
	// 	return;
	// }

	// auto SharedAnimationGroupData = GameInstance->SharedAnimationGroupDataMap.Find(Npc->Visual);
	// if (!SharedAnimationGroupData)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!SharedAnimationGroupData"));
	// 	return;
	// }

	// LOL fix this in blender...
	// auto Rotation = Npc->GetActorRotation();
	// Rotation.Roll = 0;
	// Rotation.Pitch = 0;
	// Npc->SetActorRotation(Rotation);

	// UpdateStateMachine();

	// auto AnimationData = SharedAnimationGroupData->AnimationDataMap.Find(MontageName);
	// if (!AnimationData)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!AnimationData"));
	// 	return;
	// }
}

// Real animation end
void UGHAnimInstance::OnMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted)
{

	// PlayMontage stoped all montage in same slot/group, with this function
	
	// auto AnimationElapsedSeconds = (FDateTime::Now() - AnimationStartTime).GetTotalSeconds();
	
	// Take AnimationData
	UAnimationData* StoppedAnimationData = nullptr;
	for (auto& CurrentAnimationDataTuple : CurrentAnimationDataLayerMap)
	{
		if (!CurrentAnimationDataTuple.Value)
			continue;
		
		if (CurrentAnimationDataTuple.Value->AnimMontage == Montage
			|| CurrentAnimationDataTuple.Value->AnimMontageNoRootMotion == Montage)
		{
			StoppedAnimationData = CurrentAnimationDataTuple.Value;
			break;
		}
	}

	// throw error
	// @TODO: Left and Right one moment crush...
	check(StoppedAnimationData)

	// debug message
	FString Msg = "OnMontageBlendingOutStarted, name: " + StoppedAnimationData->Name
		// + ", elapsed: " + FString::SanitizeFloat(AnimationElapsedSeconds, 3)
		+ ", interrupted: " + FString(bInterrupted ? "true" : "false");
	UE_LOG(SkAnimation, Warning, TEXT("%s"), *Msg);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Msg);

	AnimationHistoryEnd(StoppedAnimationData, bInterrupted);

	// if bInterrupted, then just becouse play some montage in same group/slot
	if (bInterrupted)
		return;

	// Layer "1" must be always if StateMachine is run
	auto Layer = StoppedAnimationData->Layer;
	if (Layer == "1")
	{
		if (bIsRunStateMachine)
			UpdateStateMachine(0, true);
		else
			CurrentAnimationDataLayerMap[Layer] = nullptr;
	}
	else
	{
		CurrentAnimationDataLayerMap[Layer] = nullptr;
	}
	
	
	
	// AnimationFinishStateMachine(StoppedAnimationData, bInterrupted);

	//
	// // Action end check, need in to func
	// if (StoppedAnimationData->Name == "T_1H_2_1HRUN" || StoppedAnimationData->Name == "T_MOVE_2_1HMOVE")
	// {
	// 	Npc->ActionQueue->DrawWeaponMeleeActionEnd();
	// }
	// else if (StoppedAnimationData->Name == "T_1HMOVE_2_MOVE")
	// {
	// 	Npc->ActionQueue->RemoveWeaponMeleeActionEnd();
	// }
	// else if (StoppedAnimationData->Name == "T_BOW_2_BOWRUN" || StoppedAnimationData->Name == "T_MOVE_2_BOWMOVE")
	// {
	// 	Npc->ActionQueue->DrawWeaponRangeActionEnd();
	// }
	// else if (StoppedAnimationData->Name == "T_BOWMOVE_2_MOVE")
	// {
	// 	Npc->ActionQueue->RemoveWeaponRangeActionEnd();
	// }
	//
	// // if don't change CurrentAnimationDataLayerMap cant work "play same = false" anims
	// // like remove weapon
	// if (StoppedAnimationData->Layer != "1")
	// {
	// 	auto Layer = StoppedAnimationData->Layer;
	// 	// Don't need StateMachine on other layers
	// 	if (CurrentAnimationDataLayerMap.Contains(Layer) && PreviousAnimationDataLayerMap.Contains(Layer))
	// 	{
	// 		PreviousAnimationDataLayerMap[Layer] = CurrentAnimationDataLayerMap[Layer];
	// 		CurrentAnimationDataLayerMap[Layer] = nullptr;
	// 	}
	// 	
	// 	// Need stop anim, because anim freeze
	// 	// this not work...
	// 	// Montage_Stop(CurrentAnimationDataLayerMap[Layer].BlendOut, Montage);
	//
	// 	// return;
	// }
	//
	// // hardcode, no check next animation
	// if (StoppedAnimationData->Layer == "20")
	// {
	// 	if (StoppedAnimationData == TurnLeftAnimationData)
	// 	{
	// 		if (bIsTurnLeftPressed)
	// 		{
	// 			PlayMontage(TurnLeftAnimationData, false, true, false);
	// 		}
	// 		else
	// 		{
	// 			StopMontage(TurnLeftAnimationData);
	// 		}
	//
	// 		return;
	// 	}
	// 	// // layers need state machine...
	// 	// if (CurrentAnimationData->Name.ToUpper().Contains("TURN"))
	// 	// {
	// 	// 	PlayMontage(*CurrentAnimationData, false, true, false);
	// 	//
	// 	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "RESET TURN");
	// 	//
	// 	// 	return;
	// 	// }
	// }
	//
	// AnimationHistoryEnd(AnimationElapsedSeconds, bInterrupted);
	//
	// if (AnimationDataQueueArray.Num() > 0)
	// {
	// 	if (Montage == AnimationDataQueueArray[0]->AnimMontage ||
	// 		Montage == AnimationDataQueueArray[0]->AnimMontageNoRootMotion)
	// 	{
	// 		AnimationDataQueueArray.RemoveAt(0, 1, false);
	//
	// 		if (AnimationDataQueueArray.Num() > 0)
	// 		{
	// 			// RM check?
	// 			PlayMontage(AnimationDataQueueArray[0]);
	// 			return;
	// 		}
	// 	}
	// }
	//
	// if (Npc->ControlledBy == EControlledBy::Player)
	// {
	// 	if (Npc->WalkMode == EWalkMode::Run)
	// 	{
	// 		if (!bInterrupted)
	// 		{
	// 			// проверяем те что без прокладки, и БЕЗ транзишинами
	// 			if (CurrentAnimationDataLayerMap["1"] == MoveForwardAnimationData)
	// 			{}
	// 			if (CurrentAnimationDataLayerMap["1"] == MoveLeftAnimationData)
	// 			{}
	// 			else if (CurrentAnimationDataLayerMap["1"] == MoveRightAnimationData)
	// 			{}
	// 			// проверяем те что без прокладки, и НО С транзишинами
	// 			else if (CurrentAnimationDataLayerMap["1"]->NextAnimation == MoveForwardAnimationData->Name)
	// 			{
	// 				if (bIsMoveForwardPressed)
	// 					PlayMontage(MoveForwardAnimationData); // return??
	// 				else
	// 				{
	// 					ResetStateMachine();
	// 				}
	// 			}
	// 			// это получается с прокладкой
	// 			else
	// 			{
	// 				ResetStateMachine();
	// 			}
	// 	
	// 			ChooseMoveDirection(true);
	// 		}
	// 	}
	// 	else if (Npc->WalkMode == EWalkMode::Walk)
	// 	{
	// 		if (!bInterrupted)
	// 		{
	// 			// TODO: in move backward use two trasnsitions
	// 			
	// 			// проверяем те что без прокладки, и БЕЗ транзишинами
	// 			if (CurrentAnimationDataLayerMap["1"] == MoveForwardAnimationData)
	// 			{}
	// 			if (CurrentAnimationDataLayerMap["1"] == MoveBackwardAnimationData)
	// 			{}
	// 			if (CurrentAnimationDataLayerMap["1"] == MoveLeftAnimationData)
	// 			{}
	// 			else if (CurrentAnimationDataLayerMap["1"] == MoveRightAnimationData)
	// 			{}
	// 			// проверяем те что без прокладки, и НО С транзишинами
	// 			else if (CurrentAnimationDataLayerMap["1"]->NextAnimation == MoveForwardAnimationData->Name)
	// 			{
	// 				if (bIsMoveForwardPressed)
	// 					PlayMontage(MoveForwardAnimationData); // return??
	// 				else
	// 				{
	// 					ResetStateMachine();
	// 				}
	//
	// 			}
	// 			else if (CurrentAnimationDataLayerMap["1"]->NextAnimation == MoveBackwardAnimationData->Name)
	// 			{
	// 				if (bIsMoveBackwardPressed)
	// 					PlayMontage(MoveBackwardAnimationData); // return??
	// 				else
	// 				{
	// 					ResetStateMachine();
	// 				}
	//
	// 			}
	// 			// это получается с прокладкой
	// 			else
	// 			{
	// 				ResetStateMachine();
	// 			}
	// 	
	// 			ChooseMoveDirection(true);
	// 		}
	// 	}
	// }
	// else if (Npc->ControlledBy == EControlledBy::AI)
	// {
	// 	if (!bInterrupted)
	// 	{
	// 		if (GroundSpeed > 2.0f)
	// 		{
	// 			PlayMontage(MoveForwardAnimationData, false, true, true);
	// 		}
	// 		else
	// 		{
	// 			PlayMontage(IdleAnimationData, false, true, false);
	// 		}
	// 	}
	// }
}

void UGHAnimInstance::OnMontageChangeSection()
{
	// UE_LOG(SkAnimation, Warning, TEXT("OnMontageChangeSection"));
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	// 	"OnMontageChangeSection: " + FString::SanitizeFloat(CurrentMontagePosition, 3));

	// if (!CurrentMontage)
	// 	return;
	//
	// if (CurrentMontage->GetName() == MoveForwardAnimationName)
	// {
	// 	if (!bIsMoveForwardPressed)
	// 	{
	// 		ResetStateMachine(MoveForwardAnimationName);
	// 	}
	// }
}

void UGHAnimInstance::OnEventTrigged(const FEventData& EventData)
{
	auto NotifyEntry = FString::Printf(TEXT("play anim, events count: %s"), *EventData.Name);
	
	// TODO: add all events
	if (EventData.Name.Compare("eventSFXGrnd", ESearchCase::IgnoreCase) == 0)
	{
		// auto EventMessage = FString::Printf(TEXT("Event. Name: eventSFXGrnd, Effect: %s"), *EventData.Effect);
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, EventMessage);
		
		if (EventData.Effect == "RUN")
		{
			Npc->PlayStepSound();
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, NotifyEntry + " RUN");
		}
	}
	else if (EventData.Name.Compare("eventTag", ESearchCase::IgnoreCase) == 0)
	{
		auto EventMessage = FString::Printf(TEXT("Event. Name: eventTag, Tag: %s, Arg1: %s"),
			*EventData.Tag, *EventData.Arg1);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, EventMessage);
		
		if (EventData.Tag.Compare("DEF_FIGHTMODE", ESearchCase::IgnoreCase) == 0)
		{
			// @TODO: change npc fightmode
			if (EventData.Arg1.Compare("1H", ESearchCase::IgnoreCase) == 0)
			{
				Npc->SwapWeaponMesh(true);
				Npc->AnimInstance->UpdateAnimationPreset(EAnimationPreset::Weapon1H);
			}
			else if (EventData.Arg1.Compare("Bow", ESearchCase::IgnoreCase) == 0)
			{
				Npc->SwapWeaponMesh(true);
				Npc->AnimInstance->UpdateAnimationPreset(EAnimationPreset::Bow);
			}
			else
			{
				Npc->SwapWeaponMesh(false);
				Npc->AnimInstance->UpdateAnimationPreset();
			}
		}
	}
}

void UGHAnimInstance::AddAnimNotifyToAnimMontage(UAnimMontage* AnimMontage)
{
	FName NotifyName = "NotifyName";
	
	auto AnimNotify = NewObject<UGHAnimNotify>(AnimMontage, UGHAnimNotify::StaticClass(), NAME_None, RF_Transactional);
	int32 my_index = AnimMontage->Notifies.Add(FAnimNotifyEvent());
	FAnimNotifyEvent& AnimNotifyEvent = AnimMontage->Notifies[my_index];

	// Here you set the necessary information for the Custom Class.
	// I need "Notify" index and the name I defined.
	// AnimNotify->NotifyName = NAME_None;
	// AnimNotify->set_notify_index(my_index);
	
	AnimNotifyEvent.Notify = AnimNotify;
	AnimNotifyEvent.Link(AnimMontage, 0.0f);
}

bool UGHAnimInstance::CanNotifyUseTrack(const FAnimNotifyTrack& Track, const FAnimNotifyEvent& Notify)
{
	for (const FAnimNotifyEvent* Event : Track.Notifies)
	{
		if (FMath::IsNearlyEqual(Event->GetTime(), Notify.GetTime()))
		{
			return false;
		}
	}
	return true;
}

FAnimNotifyTrack& UGHAnimInstance::AddNewTrack(TArray<FAnimNotifyTrack>& Tracks)
{
	const int32 Index = Tracks.Add(FAnimNotifyTrack(*FString::FromInt(Tracks.Num() + 1), FLinearColor::White));
	return Tracks[Index];
}

void UGHAnimInstance::AddAnimNotifyToAnimSequence(UAnimSequence* AnimSequence)
{
	float StartTime = 0.01f;
	float Duration = 0.01f;
	
	if (!AnimSequence)
		return;
	
	// auto TrackIndex = AnimSequence->AnimNotifyTracks.IndexOfByPredicate([&](const FAnimNotifyTrack& Track)
	// {
	// 	// just return index of first track	
	// 	return true;
	// });
	//
	// if (TrackIndex == INDEX_NONE)
	// 	return;

	auto bIsValidTime = FMath::IsWithinInclusive(StartTime, 0.0f, AnimSequence->GetPlayLength());
	if (!bIsValidTime)
		return;

	auto NotifyState = NewObject<UAnimNotifyState>(AnimSequence, UAnimNotifyState::StaticClass(),
		NAME_None, RF_Transactional);
	if (!NotifyState)
		return;

	auto NewEvent = FAnimNotifyEvent();

	NewEvent.NotifyName = NAME_None;
	NewEvent.Link(AnimSequence, StartTime);
	// NewEvent.TriggerTimeOffset = GetTriggerTimeOffsetForType(AnimSequence->CalculateOffsetForNotify(StartTime));
	NewEvent.TriggerTimeOffset = 0.0f;
	NewEvent.TrackIndex = 0;
	NewEvent.Notify = nullptr;
	NewEvent.Guid = FGuid::NewGuid();

	NewEvent.NotifyStateClass = NotifyState;
	NewEvent.NotifyName = FName("Test NotifyState");
	NewEvent.SetDuration(Duration);
	NewEvent.EndLink.Link(AnimSequence, NewEvent.EndLink.GetTime());


	AnimSequence->SortSyncMarkers();

	for (int32 TrackIndex = 0; TrackIndex < AnimSequence->AnimNotifyTracks.Num(); ++TrackIndex)
	{
		AnimSequence->AnimNotifyTracks[TrackIndex].SyncMarkers.Empty();
	}
	for (FAnimSyncMarker& SyncMarker : AnimSequence->AuthoredSyncMarkers)
	{
		const int32 TrackIndex = SyncMarker.TrackIndex;
		if (AnimSequence->AnimNotifyTracks.IsValidIndex(TrackIndex))
		{
			AnimSequence->AnimNotifyTracks[TrackIndex].SyncMarkers.Add(&SyncMarker);
		}
		else
		{
			// This should not happen, but if it does we must find somewhere else to add it
			// ensureMsgf(0, TEXT("AnimNotifyTrack: Wrong indices found"));
			AnimSequence->AnimNotifyTracks[0].SyncMarkers.Add(&SyncMarker);
			SyncMarker.TrackIndex = 0;
		}
	}

	AnimSequence->SortNotifies();

	for (int32 TrackIndex = 0; TrackIndex < AnimSequence->AnimNotifyTracks.Num(); ++TrackIndex)
	{
		AnimSequence->AnimNotifyTracks[TrackIndex].Notifies.Empty();
	}

	for (FAnimNotifyEvent& Notify : AnimSequence->Notifies)
	{
		// Handle busted track indices
		if (!AnimSequence->AnimNotifyTracks.IsValidIndex(Notify.TrackIndex))
		{
			// This really shouldn't happen (unless we are a cooked asset), but try to handle it
			// ensureMsgf(GetOutermost()->bIsCookedForEditor, TEXT("AnimNotifyTrack: Anim (%s) has notify (%s) with track index (%i) that does not exist"), *GetFullName(), *Notify.NotifyName.ToString(), Notify.TrackIndex);

			// Don't create lots of extra tracks if we are way off supporting this track
			if (Notify.TrackIndex < 0 || Notify.TrackIndex > 20)
			{
				Notify.TrackIndex = 0;
			}

			while (!AnimSequence->AnimNotifyTracks.IsValidIndex(Notify.TrackIndex))
			{
				AddNewTrack(AnimSequence->AnimNotifyTracks);
			}
		}

		// Handle overlapping notifies
		FAnimNotifyTrack* TrackToUse = nullptr;
		int32 TrackIndexToUse = INDEX_NONE;
		for (int32 TrackOffset = 0; TrackOffset < AnimSequence->AnimNotifyTracks.Num(); ++TrackOffset)
		{
			const int32 TrackIndex = (Notify.TrackIndex + TrackOffset) % AnimSequence->AnimNotifyTracks.Num();
			if (CanNotifyUseTrack(AnimSequence->AnimNotifyTracks[TrackIndex], Notify))
			{
				TrackToUse = &AnimSequence->AnimNotifyTracks[TrackIndex];
				TrackIndexToUse = TrackIndex;
				break;
			}
		}

		if (TrackToUse == nullptr)
		{
			TrackToUse = &AddNewTrack(AnimSequence->AnimNotifyTracks);
			TrackIndexToUse = AnimSequence->AnimNotifyTracks.Num() - 1;
		}

		check(TrackToUse);
		check(TrackIndexToUse != INDEX_NONE);

		Notify.TrackIndex = TrackIndexToUse;
		TrackToUse->Notifies.Add(&Notify);
	}

	// this is a separate loop of checkin if they contains valid notifies
	for (int32 NotifyIndex = 0; NotifyIndex < AnimSequence->Notifies.Num(); ++NotifyIndex)
	{
		const FAnimNotifyEvent& Notify = AnimSequence->Notifies[NotifyIndex];
		// make sure if they can be placed in editor
		if (Notify.Notify)
		{
			if (Notify.Notify->CanBePlaced(AnimSequence) == false)
			{
				// static FName NAME_LoadErrors("LoadErrors");
				// FMessageLog LoadErrors(NAME_LoadErrors);
				//
				// TSharedRef<FTokenizedMessage> Message = LoadErrors.Error();
				// Message->AddToken(FTextToken::Create(LOCTEXT("InvalidAnimNotify1", "The Animation ")));
				// Message->AddToken(FAssetNameToken::Create(GetPathName(), FText::FromString(GetNameSafe(this))));
				// Message->AddToken(FTextToken::Create(LOCTEXT("InvalidAnimNotify2", " contains invalid notify ")));
				// Message->AddToken(FAssetNameToken::Create(Notify.Notify->GetPathName(), FText::FromString(GetNameSafe(Notify.Notify))));
				// LoadErrors.Open();

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "ERRROR");
			}
		}

		if (Notify.NotifyStateClass)
		{
			if (Notify.NotifyStateClass->CanBePlaced(AnimSequence) == false)
			{
				// static FName NAME_LoadErrors("LoadErrors");
				// FMessageLog LoadErrors(NAME_LoadErrors);
				//
				// TSharedRef<FTokenizedMessage> Message = LoadErrors.Error();
				// Message->AddToken(FTextToken::Create(LOCTEXT("InvalidAnimNotify1", "The Animation ")));
				// Message->AddToken(FAssetNameToken::Create(GetPathName(), FText::FromString(GetNameSafe(this))));
				// Message->AddToken(FTextToken::Create(LOCTEXT("InvalidAnimNotify2", " contains invalid notify ")));
				// Message->AddToken(FAssetNameToken::Create(Notify.NotifyStateClass->GetPathName(), FText::FromString(GetNameSafe(Notify.NotifyStateClass))));
				// LoadErrors.Open();

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "ERRROR");
			}
		}
	}
	// notification broadcast
	// AnimSequence->OnNotifyChanged.Broadcast();

	// Super::RefreshCacheData();
	
	// Refresh all cached data
	// AnimSequence->RefreshCacheData();

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AddAnimNotifyToAnimSequence: OK");
}

FString UGHAnimInstance::CheckHardCodeAnimation(const FString& Name)
{
	if (Name == "T_WALKBL_2_WALK")
		return "T_WALKBR_2_WALK";

	return Name;
}

void UGHAnimInstance::AnimationHistoryAdd(FString& Name, bool bRootMotion)
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimationHistorySet: " + Name);
	
	// auto AnimationHistoryMaxLen = 20;
	// auto AnimationHistoryCurLen = AnimationHistoryArray.Num();
	// if (AnimationHistoryCurLen >= AnimationHistoryMaxLen && AnimationHistoryCurLen > 0)
	// 	AnimationHistoryArray.RemoveAt(0, 1, false);
	//
	// auto AnimationHistory = FAnimationHistory();
	// AnimationHistory.Name = Name;
	//
	// AnimationHistoryArray.Add(AnimationHistory);


	
	// auto AnimationHistoryCurLen = AnimationHistoryArray.Num();
	// if (AnimationHistoryCurLen == 1)
	// {
	// 	auto AnimationHistoryNew = FAnimationHistory();
	// 	AnimationHistoryNew.Name = Name;
	// 	AnimationHistoryNew.bRootMotion = bRootMotion;
	// 	AnimationHistoryArray.Add(AnimationHistoryNew);
	// }
	// else if (AnimationHistoryCurLen > 0)
	// {
	// 	if (AnimationHistoryArray[AnimationHistoryCurLen - 1].Name.IsEmpty())
	// 	{
	// 		if (AnimationHistoryArray[AnimationHistoryCurLen - 1].Duration >= 0.001f)
	// 		{
	// 			AnimationHistoryArray.Add(FAnimationHistory());
	// 		}
	// 	}
	//
	// 	AnimationHistoryArray.Last().Name = Name;
	// 	AnimationHistoryArray.Last().bRootMotion = bRootMotion;
	// }

	auto AnimationHistory = FAnimationHistory();
	AnimationHistory.Name = Name;
	AnimationHistory.bRootMotion = bRootMotion;
	AnimationHistoryArray.Add(AnimationHistory);
}

void UGHAnimInstance::AnimationHistoryRemove()
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimationHistoryRemove: ");
	
	// auto AnimationHistoryMaxLen = 20;
	// auto AnimationHistoryCurLen = AnimationHistoryArray.Num();
	// if (AnimationHistoryCurLen >= AnimationHistoryMaxLen && AnimationHistoryCurLen > 0)
	// 	AnimationHistoryArray.RemoveAt(0, 1, false);
	//
	// auto AnimationHistory = FAnimationHistory();
	// AnimationHistory.Name = Name;
	//
	// AnimationHistoryArray.Add(AnimationHistory);

	auto AnimationHistoryCurLen = AnimationHistoryArray.Num();
	if (AnimationHistoryCurLen > 0)
	{
		AnimationHistoryArray.RemoveAt(AnimationHistoryCurLen - 1, 1, false);
	}
}

void UGHAnimInstance::AnimationHistoryEnd(UAnimationData* AnimationData, bool bInterrupted)
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimationHistoryEnd: ");
	
	// auto AnimationHistoryCurLen = AnimationHistoryArray.Num();
	// if (AnimationHistoryCurLen > 0)
	// {
	// 	if (PreviousAnimationDataLayerMap["1"] == CurrentAnimationDataLayerMap["1"])
	// 	{
	// 		AnimationHistoryArray[AnimationHistoryCurLen - 1].Duration += Elapsed;
	// 	}
	// 	else
	// 	{
	// 		AnimationHistoryArray[AnimationHistoryCurLen - 1].Duration = Elapsed;
	// 		AnimationHistoryArray[AnimationHistoryCurLen - 1].bInterrupted = bInterrupted;
	//
	// 		auto AnimationHistoryMaxLen = 20;
	// 		if (AnimationHistoryCurLen >= AnimationHistoryMaxLen && AnimationHistoryCurLen > 0)
	// 			AnimationHistoryArray.RemoveAt(0, 1, false);
	// 			
	// 		AnimationHistoryArray.Add(FAnimationHistory());
	// 	}
	// }

	for (int32 InstanceIndex = MontageInstances.Num() - 1; InstanceIndex >= 0; InstanceIndex--)
	{
		auto MontageInstance = MontageInstances[InstanceIndex];
		if (!MontageInstance) //  || !MontageInstance->IsActive()
			continue;
		
		if (!(MontageInstance->Montage == AnimationData->AnimMontage
			|| MontageInstance->Montage == AnimationData->AnimMontageNoRootMotion))
			continue;

		for (int32 i = AnimationHistoryArray.Num() - 1; i >= 0; i--)
		{
			if (AnimationHistoryArray[i].bFinished)
				continue;
			
			if (AnimationHistoryArray[i].Name == AnimationData->Name)
			{
				AnimationHistoryArray[i].Duration = MontageInstance->GetPosition();
				AnimationHistoryArray[i].bFinished = true;
				AnimationHistoryArray[i].bInterrupted = bInterrupted;
				
				return;
			}
		}
	}
}

void UGHAnimInstance::AnimationHistoryUpdate(float Elapsed)
{
	// auto AnimationHistoryCurLen = AnimationHistoryArray.Num();
	// if (AnimationHistoryCurLen > 0)
	// {
	// 	AnimationHistoryArray[AnimationHistoryCurLen - 1].Duration += Elapsed;
	// }
}

int32 UGHAnimInstance::AnimMontageAddSection(UAnimMontage* AnimMontage, FName InSectionName, float StartTime, FName NextSectionName)
{
	if (InSectionName == NAME_None)
	{
		UE_LOG(SkAnimation, Warning, TEXT("InSectionName == NAME_None"));
		return INDEX_NONE;
	}
	
	if (AnimMontage->GetSectionIndex(InSectionName) != INDEX_NONE)
	{
		UE_LOG(SkAnimation, Warning, TEXT("Montage->GetSectionIndex(InSectionName) != INDEX_NONE"));
		return INDEX_NONE;
	}

	FCompositeSection NewSection;
	NewSection.SectionName = InSectionName;
	NewSection.Link(AnimMontage, StartTime / 1000.0f);

	// we'd like to sort them in the order of time
	int32 NewSectionIndex = AnimMontage->CompositeSections.Add(NewSection);

	if (NextSectionName != NAME_None)
	{
		auto NextSectionID = AnimMontage->GetSectionIndex(NextSectionName);
		auto bNextSectionIsValid = AnimMontage->CompositeSections.IsValidIndex(NextSectionID);
		if (bNextSectionIsValid)
		{
			AnimMontage->CompositeSections[NewSectionIndex].NextSectionName = NextSectionName;
		}
	}
	{
		// do not use this, have some bugs...
		
		// // when first added, just make sure to link previous one to add me as next if previous one doesn't have any link
		// // it's confusing first time when you add this data
		// int32 PrevSectionIndex = NewSectionIndex - 1;
		// if (AnimMontage->CompositeSections.IsValidIndex(PrevSectionIndex))
		// {
		// 	if (AnimMontage->CompositeSections[PrevSectionIndex].NextSectionName == NAME_None)
		// 	{
		// 		AnimMontage->CompositeSections[PrevSectionIndex].NextSectionName = InSectionName;
		// 	}
		// }
	}

	return NewSectionIndex;
}

bool UGHAnimInstance::SetupAnimationData(UAnimationData* AnimationData, bool bRootMotion)
{
	if (!AnimationData || !AnimationData->IsValid())
		return false;
	
	if (bRootMotion)
	{
		if (!AnimationData->AnimMontage)
		{
			AnimationData->AnimMontage = CreateAnimMontageFromAnimSequence(AnimationData->AnimSequence, AnimationData);
			if (!AnimationData->AnimMontage)
				return false;
			
			// AnimationData.AnimMontage->Rename(*(AnimationData.Name + "_ROOT_MOTION"));
		}
	}
	else
	{
		if (!AnimationData->AnimSequenceNoRootMotion)
		{
			AnimationData->AnimSequenceNoRootMotion = DuplicateObject<UAnimSequence>(AnimationData->AnimSequence, this);
			if (!AnimationData->AnimSequenceNoRootMotion)
				return false;

			AnimationData->AnimSequenceNoRootMotion->bEnableRootMotion = false;
			AnimationData->AnimSequenceNoRootMotion->bForceRootLock = true;
		}

		if (!AnimationData->AnimMontageNoRootMotion)
		{
			AnimationData->AnimMontageNoRootMotion = CreateAnimMontageFromAnimSequence(
				AnimationData->AnimSequenceNoRootMotion, AnimationData);
			if (!AnimationData->AnimMontageNoRootMotion)
				return false;
			
			// AnimationData.AnimMontageNoRootMotion->Rename(*(AnimationData.Name + "_NO_ROOT_MOTION"));
		}
		
		// if (AnimationData.AnimMontageNoRootMotion)
		// 	return AnimationData->AnimMontageNoRootMotion;
		//
		// if (!AnimationData->AnimSequence)
		// 	return nullptr;
		//
		// AnimationData->AnimSequenceNoRootMotion = DuplicateObject<UAnimSequence>(AnimationData->AnimSequence,this);
		// if (!AnimationData->AnimSequenceNoRootMotion)
		// 	return nullptr;
		//
		// AnimationData->AnimSequenceNoRootMotion->bEnableRootMotion = false;
		// AnimationData->AnimSequenceNoRootMotion->bForceRootLock = true;
		//
		// AnimationData->AnimMontageNoRootMotion = CreateAnimMontageFromAnimSequence(
		// 	AnimationData->AnimSequenceNoRootMotion, *AnimationData);
		// if (!AnimationData->AnimMontageNoRootMotion)
		// 	return nullptr;
		//
		// AnimationData->AnimMontageNoRootMotion->Rename(*(AnimationData->Name + "_NO_ROOT_MOTION"));
	}

	return true;
}

UAnimMontage* UGHAnimInstance::CreateAnimMontageFromAnimSequence(UAnimSequence* AnimSequence, UAnimationData* AnimationData)
{
	if (!AnimSequence || !AnimationData)
		return nullptr;

	// in loop blending not work :(
	auto LoopCount = 1;

	// S_RUNL flicks, now blend out not support
	auto BlendIn = AnimationData->BlendIn;
	if (AnimationData->NextAnimation == AnimationData->Name)
	{
		BlendIn = AnimationData->BlendIn + AnimationData->BlendOut;
		// LoopCount = 999999; // in loop blending not work :(
	}
	
	FMontageBlendSettings BlendInSettings(BlendIn);
	
	// Blend out option. This is only used when it blends out itself.
	// If it's interrupted by other montages, it will use new montage's BlendIn option to blend out.
	// Value measured in seconds.
	// FMontageBlendSettings BlendOutSettings(AnimationData->BlendOut);
	// @TODO: check all layers behavior
	auto BlendOutTime = AnimationData->Layer == "1" ? 20.0f : AnimationData->BlendOut;
	// BlendOutTime = AnimationData->Layer == "20" ? 20.0f : AnimationData->BlendOut;
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimationData->Name: " + AnimationData->Name);
	
	// if (AnimationData->Name.Compare("T_RUNTURNL", ESearchCase::IgnoreCase) == 0)
	// {
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "T_RUNTURNL 20 !!!!!!!!!!!!!");
	// 	BlendOutTime = 20.0f;
	// }

	// Try all anim BlendOutTime 20s
	BlendOutTime = 20.0f;
	
	FMontageBlendSettings BlendOutSettings(BlendOutTime); // emulate pause on anim end

	// Time from Sequence End to trigger blend out.
	// <0 means using BlendOutTime, so BlendOut finishes as Montage ends. (include)
	// >=0 means using 'SequenceEnd - BlendOutTriggerTime' to trigger blend out. (offset)
	// start blendout event -> w8 (blendout time - this param) -> start anim end event
	float BlendOutTriggerTime = 0.020; // little offser for "perfect seam" (anti lag) 20 ms.

	// TODO: check multicreate
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "-----------------------------------------------------create: " + AnimationData.Name);

	// UAnimMontage* AnimMontage = nullptr;
	auto MontageSlot = FName("S" + AnimationData->Layer);
	FString DebugString = MontageSlot.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "DebugString: " + DebugString);
	auto AnimMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage_WithBlendSettings(AnimSequence,
		MontageSlot, BlendInSettings, BlendOutSettings,
		1, LoopCount, BlendOutTriggerTime);

	if (!AnimMontage)
		return nullptr;

	// For convenience / comfortable
	FString NewObjectName = AnimationData->Name + "_" + AnimMontage->GetName();
	AnimMontage->Rename(*NewObjectName);

	// // Must be unique name, or crush
	// AnimMontage->Rename((*AnimationData.Name));

	// // When it hits end, it automatically blends out.
	// // If this is false, it won't blend out but keep the last pose until stopped explicitly.
	// AnimMontage->bEnableAutoBlendOut = false;

	// FName NextSectionName = NAME_None;
	// if (/*AnimationData->Flags.Contains("I") &&*/ AnimationData.NextAnimation == AnimationData.Name)
	// 	NextSectionName = FName("section_start");
	//
	// // alternative: https://docs.unrealengine.com/5.0/en-US/API/Runtime/Engine/Animation/FAnimSegment/
	// // time in ms
	// AnimMontageAddSection(AnimMontage, FName("section_start"), AnimationData.StartTime, NextSectionName);
	// AnimMontageAddSection(AnimMontage, FName("section_end"), AnimationData.EndTime);
	
	return AnimMontage;
}

void UGHAnimInstance::UpdateAnimationPreset(EAnimationPreset EAnimationPreset, bool Update)
{
	auto WalkModeString = UEnum::GetValueAsString(Npc->WalkMode);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UpdateAnimationPreset: " + WalkModeString);
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Visual: " + Npc->Visual);
	

	if (EAnimationPreset == EAnimationPreset::None)
	{
		// todo crush if not find, just change name
		if (Npc->WalkMode == EWalkMode::Walk)
		{
			IdleAnimationData = GetAnimationDataByName("S_WALK");
			MoveForwardAnimationData = GetAnimationDataByName("S_WALKL"); // 1.3 dist
			MoveBackwardAnimationData = GetAnimationDataByName("S_WALKBL");
			MoveLeftAnimationData = GetAnimationDataByName("T_WALKSTRAFEL");
			MoveRightAnimationData = GetAnimationDataByName("T_WALKSTRAFER");
			TurnLeftAnimationData = GetAnimationDataByName("T_WALKTURNL");
			TurnRightAnimationData = GetAnimationDataByName("T_WALKTURNR");
		}
		else if (Npc->WalkMode == EWalkMode::Run)
		{
			IdleAnimationData = GetAnimationDataByName("S_RUN");
			MoveForwardAnimationData = GetAnimationDataByName("S_RUNL");
			MoveBackwardAnimationData = GetAnimationDataByName("T_JUMPB");
			MoveLeftAnimationData = GetAnimationDataByName("T_RUNSTRAFEL");
			MoveRightAnimationData = GetAnimationDataByName("T_RUNSTRAFER");
			TurnLeftAnimationData = GetAnimationDataByName("T_RUNTURNL");
			TurnRightAnimationData = GetAnimationDataByName("T_RUNTURNR");
		}
		else
		{
			IdleAnimationData = GetAnimationDataByName("S_RUN");
			MoveForwardAnimationData = GetAnimationDataByName("S_RUNL");
			MoveBackwardAnimationData = GetAnimationDataByName("T_JUMPB");
			MoveLeftAnimationData = GetAnimationDataByName("T_RUNSTRAFEL");
			MoveRightAnimationData = GetAnimationDataByName("T_RUNSTRAFER");
			TurnLeftAnimationData = GetAnimationDataByName("T_RUNTURNL");
			TurnRightAnimationData = GetAnimationDataByName("T_RUNTURNR");
		}
	}
	else if (EAnimationPreset == EAnimationPreset::Bow)
	{
		if (Npc->WalkMode == EWalkMode::Run)
		{
			IdleAnimationData = GetAnimationDataByName("S_BOWRUN");
			MoveForwardAnimationData = GetAnimationDataByName("S_BOWRUNL");
			MoveBackwardAnimationData = GetAnimationDataByName("T_BOWJUMPB");
			MoveLeftAnimationData = GetAnimationDataByName("T_BOWRUNSTRAFEL");
			MoveRightAnimationData = GetAnimationDataByName("T_BOWRUNSTRAFER");
		}
	}
	else if (EAnimationPreset == EAnimationPreset::Weapon1H)
	{
		if (Npc->WalkMode == EWalkMode::Run)
		{
			IdleAnimationData = GetAnimationDataByName("S_1HRUN");
			MoveForwardAnimationData = GetAnimationDataByName("S_1HRUNL");
			MoveBackwardAnimationData = GetAnimationDataByName("T_1HPARADEJUMPB");
			MoveLeftAnimationData = GetAnimationDataByName("T_1HRUNSTRAFEL");
			MoveRightAnimationData = GetAnimationDataByName("T_1HRUNSTRAFER");
		}
	}
	
	
	// if (!IdleAnimationData.IsValid() || !MoveForwardAnimationData.IsValid())
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimationData NOT VALID");
	// }

	// AnimMontageIdle = GetAnimMontageByName(IdleAnimationName, false);
	// AnimMontageMoveForward = GetAnimMontageByName(MoveForwardAnimationName, false);
	// if (AnimMontageMoveForward)
	// {
	// 	auto BlendInSettings = FAlphaBlendArgs(0.25f);
	// 	auto Lenght = Montage_PlayWithBlendIn(AnimMontageMoveForward, BlendInSettings, 1.0f,
	// 	EMontagePlayReturnType::MontageLength, (480 / 1000.0f) + 0.001f, true);
	// 	if (Lenght > 0.0f)
	// 	{
	// 		// CurrentMontage = AnimationData->AnimMontage;
	// 		// CurrentAnimationName = AnimationData->Name;
	// 		// NextAnimationName = AnimationData->NextAnimation;
	// 		
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "anim lenght: " + FString::SanitizeFloat(Lenght, 3));
	// 	}
	// }
	// else
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AnimMontageMoveForward: ERR");
	// }
}

void UGHAnimInstance::RunStateMachine()
{
	if (Npc->Visual.IsEmpty())
		return;
	
	bIsRunStateMachine = true;
	
	if (Npc->ControlledBy == EControlledBy::Player)
		PlayMontage(IdleAnimationData, true, true);
	else if (Npc->ControlledBy == EControlledBy::AI)
	 	PlayMontage(IdleAnimationData, false, true);
}

void UGHAnimInstance::ResetStateMachine(const FString& EndedAnimationName)
{
	if (!bIsRunStateMachine)
		return;
	
	RunStateMachine();
}

// every game frame, not animation frame
void UGHAnimInstance::UpdateStateMachine(float DeltaSeconds, bool bInvokedFromAnimationFinish)
{
	// Order = left/right -> forward/backward
	// Priority up to down

	if (bInvokedFromAnimationFinish)
	{
		if (bIsMoveLeftPressed && !bIsMoveRightPressed)
		{
			PlayMontage(MoveLeftAnimationData, true, true, false);
		}
		else if (!bIsMoveLeftPressed && bIsMoveRightPressed)
		{
			PlayMontage(MoveRightAnimationData, true, true, false);
		}
		// Left & Right pressed = counter-block
		else if (bIsMoveLeftPressed && bIsMoveRightPressed)
		{
			PlayMontage(IdleAnimationData, false, true, false);
		}
		else if (bIsMoveForwardPressed)
		{
			// Transition?
			PlayMontage(MoveForwardAnimationData, true, true, true);
		}
		else if (bIsMoveBackwardPressed)
		{
			PlayMontage(MoveBackwardAnimationData, true, true, false);
		}
		else
		{
			ResetStateMachine();
		}
	}
	else
	{
		if (bIsMoveLeftPressed && !bIsMoveRightPressed)
		{
			// if we strafe, and push back - restart strafe...
			if (CurrentAnimationDataLayerMap[MoveLeftAnimationData->Layer] != MoveLeftAnimationData)
				PlayMontage(MoveLeftAnimationData, true, true, false);
		}
		else if (!bIsMoveLeftPressed && bIsMoveRightPressed)
		{
			// if we strafe, and push back - restart strafe...
			if (CurrentAnimationDataLayerMap[MoveRightAnimationData->Layer] != MoveRightAnimationData)
				PlayMontage(MoveRightAnimationData, true, true, false);
		}
		// Left & Right pressed = counter-block
		else if (bIsMoveLeftPressed && bIsMoveRightPressed)
		{
			PlayMontage(IdleAnimationData, false, true, false);
		}
		else if (bIsMoveForwardPressed)
		{
			PlayMontage(MoveForwardAnimationData, true, true, true);
		}
		else if (bIsMoveBackwardPressed)
		{
			if (CurrentAnimationDataLayerMap[MoveBackwardAnimationData->Layer] != MoveBackwardAnimationData)
			{
				PlayMontage(MoveBackwardAnimationData, true, true, false);
			}
		}
		else
		{
			if (CurrentAnimationDataLayerMap[MoveBackwardAnimationData->Layer] == MoveBackwardAnimationData)
			{
				// Don't stop "MoveBackward" animation
			}
			else if (CurrentAnimationDataLayerMap["1"]->Name == "T_RUN_2_RUNL")
			{
				// Don't stop "T_RUN_2_RUNL" animation
			}
			else
			{
				ResetStateMachine();
			}
		}
	}
	
	// if (bIsMoveLeftPressed && !bIsMoveRightPressed)
	// {
	// 	// if (CurrentAnimationDataLayerMap[MoveLeftAnimationData->Layer] != MoveLeftAnimationData)
	// 	PlayMontage(MoveLeftAnimationData, true, true, false);
	// }
	// else if (!bIsMoveLeftPressed && bIsMoveRightPressed)
	// {
	// 	// if (CurrentAnimationDataLayerMap[MoveRightAnimationData->Layer] != MoveRightAnimationData)
	// 	PlayMontage(MoveRightAnimationData, true, true, false);
	// }
	// // Left & Right pressed = counter-block
	// else if (bIsMoveLeftPressed && bIsMoveRightPressed)
	// {
	// 	PlayMontage(IdleAnimationData, false, true, false);
	// }
	// else if (bIsMoveForwardPressed)
	// {
	// 	PlayMontage(MoveForwardAnimationData, true, true, true);
	// }
	// else if (bIsMoveBackwardPressed)
	// {
	// 	// if (CurrentAnimationDataLayerMap[MoveBackwardAnimationData->Layer] != MoveBackwardAnimationData)
	// 	// {
	// 		PlayMontage(MoveBackwardAnimationData, true, true, false);
	// 	// }
	// }
	// else
	// {
	// 	if (CurrentAnimationDataLayerMap[MoveBackwardAnimationData->Layer] == MoveBackwardAnimationData)
	// 	{
	// 		// Don't stop "MoveBackward" animation
	// 		
	// 		if (bInvokedFromAnimationFinish)
	// 		{
	// 			if (bIsMoveBackwardPressed)
	// 			{
	// 				PlayMontage(MoveBackwardAnimationData, true, true, false);
	// 			}
	// 		}
	// 	}
	// 	else if (CurrentAnimationDataLayerMap["1"]->Name == "T_RUN_2_RUNL")
	// 	{
	// 		// Don't stop "T_RUN_2_RUNL" animation
	//
	// 		if (bInvokedFromAnimationFinish)
	// 			ResetStateMachine();
	// 	}
	// 	else
	// 	{
	// 		ResetStateMachine();
	// 	}
	// }
	

	//
	// // отмена только движений в бок (назаж и вперед не надо) !!! + нужна проkладка
	// if (CurrentAnimationDataLayerMap["1"] == MoveLeftAnimationData || CurrentAnimationDataLayerMap["1"] == MoveRightAnimationData)
	// {
	// 	ResetStateMachine();
	// }
	//
	// if (Npc->WalkMode == EWalkMode::Run)
	// {
	// 	// todo: move forward / backward interrupt
	// 	// priority up to down, forward -> backward
	// 	if (bIsMoveForwardPressed)
	// 	{
	// 		if (CurrentAnimationDataLayerMap["1"] != MoveBackwardAnimationData)
	// 			PlayMontage(MoveForwardAnimationData, true, bPlaySame);
	// 	}
	// 	else if (bIsMoveBackwardPressed)
	// 	{
	// 		PlayMontage(MoveBackwardAnimationData, true, bPlaySame);
	// 	}
	// }
	// else if (Npc->WalkMode == EWalkMode::Walk)
	// {
	// 	if (bIsMoveForwardPressed)
	// 	{
	// 		if (CurrentAnimationDataLayerMap["1"] != MoveBackwardAnimationData)
	// 			PlayMontage(MoveForwardAnimationData, true, bPlaySame);
	// 	}
	// 	else if (bIsMoveBackwardPressed)
	// 	{
	// 		PlayMontage(MoveBackwardAnimationData, true, bPlaySame);
	// 	}
	// }
	//
	// else if (!bIsMoveLeftPressed && !bIsMoveRightPressed)
	// {
	// 	// отмена только движений в бок (назаж и вперед не надо) !!! + нужна проkладка
	// 	if (CurrentAnimationDataLayerMap["1"] == MoveLeftAnimationData || CurrentAnimationDataLayerMap["1"] == MoveRightAnimationData)
	// 	{
	// 		ResetStateMachine();
	// 	}
	//
	// 	if (Npc->WalkMode == EWalkMode::Run)
	// 	{
	// 		// todo: move forward / backward interrupt
	// 		// priority up to down, forward -> backward
	// 		if (bIsMoveForwardPressed)
	// 		{
	// 			if (CurrentAnimationDataLayerMap["1"] != MoveBackwardAnimationData)
	// 				PlayMontage(MoveForwardAnimationData, true, bPlaySame);
	// 		}
	// 		else if (bIsMoveBackwardPressed)
	// 		{
	// 			PlayMontage(MoveBackwardAnimationData, true, bPlaySame);
	// 		}
	// 	}
	// 	else if (Npc->WalkMode == EWalkMode::Walk)
	// 	{
	// 		if (bIsMoveForwardPressed)
	// 		{
	// 			if (CurrentAnimationDataLayerMap["1"] != MoveBackwardAnimationData)
	// 				PlayMontage(MoveForwardAnimationData, true, bPlaySame);
	// 		}
	// 		else if (bIsMoveBackwardPressed)
	// 		{
	// 			PlayMontage(MoveBackwardAnimationData, true, bPlaySame);
	// 		}
	// 	}
	// }



	
	// if (Npc->ControlledBy == EControlledBy::AI)
	// {
	// 	if (GroundSpeed > 2.0f)
	// 	{
	// 		if (CurrentAnimationDataLayerMap["1"] == IdleAnimationData)
	// 		{
	// 			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "bShouldMove");
	// 			PlayMontage(MoveForwardAnimationData, false, false, true);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (CurrentAnimationDataLayerMap["1"] != IdleAnimationData)
	// 		{
	// 			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, " NOT bShouldMove");
	// 			PlayMontage(IdleAnimationData, false, false, false);
	// 		}
	// 	}
	// }
}

void UGHAnimInstance::AnimationFinishStateMachine(UAnimationData* AnimationData, bool bInterrupted)
{
	auto Layer = AnimationData->Layer;

	// Нужно ли нам нулить? А то есть микромомент между зацикленными анимациями, когда есть нул,
	// а анимация по сути зациклена!!!
	// PreviousAnimationDataLayerMap[Layer] = AnimationData;
	CurrentAnimationDataLayerMap[Layer] = nullptr;
	
	UpdateStateMachine();

	// if (AnimationData == TurnLeftAnimationData || AnimationData == TurnRightAnimationData)
	// {
	// 	ChooseRotateDirection();
	// }
	// if (AnimationData == MoveLeftAnimationData || AnimationData == MoveRightAnimationData)
	// {
	// 	ChooseMoveDirection();
	// }
}

FString UGHAnimInstance::ComputeTransitionAnimation(const FString& AnimationNameFrom, const FString& AnimationNameTo)
{
	if (AnimationNameFrom == AnimationNameTo)
		return "";
	
	if (AnimationNameTo.Len() <= 2)
		return "";

	if (!AnimationNameTo.StartsWith("S_"))
		return "";

	if (AnimationNameFrom.Len() <= 2)
		return "";
	
	if (!AnimationNameFrom.StartsWith("S_"))
		return "";

	auto ClearAnimationNameFrom = AnimationNameFrom.Mid(2, AnimationNameFrom.Len() - 2);
	auto ClearAnimationNameTo = AnimationNameTo.Mid(2, AnimationNameTo.Len() - 2);
	
	return "T_" + ClearAnimationNameFrom + "_2_" + ClearAnimationNameTo;
}

FString UGHAnimInstance::GetTransitionTargetAnimationName(const FString& AnimationName)
{
	if (!AnimationName.Contains("_2_"))
		return "";
	
	FString AnimationNameFrom, AnimationNameTarget;
	AnimationName.Split("_2_",&AnimationNameFrom, &AnimationNameTarget);
	if (AnimationNameTarget.IsEmpty())
		return "";

	return "S_" + AnimationNameTarget;
}

UAnimSequence* UGHAnimInstance::GetAnimSequenceByName(const FString& AnimationName)
{
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return nullptr;
	}
	
	if (!Npc)
	{
		UE_LOG(SkAnimation, Warning, TEXT("!Npc"));
		return nullptr;
	}

	auto AnimationData = GameInstance->AssetManager->GetAnimationV2(Npc->Visual, AnimationName);
	if (!AnimationData)
	{
		UE_LOG(SkAnimation, Warning, TEXT("!AnimationData"));
		return nullptr;
	}

	if (!AnimationData->AnimSequence)
	{
		UE_LOG(SkAnimation, Warning, TEXT("!AnimationData->AnimSequence"));
		return nullptr;
	}
	
	return AnimationData->AnimSequence;
}

UAnimMontage* UGHAnimInstance::GetAnimMontageByName(const FString& AnimationName, bool bRootMotion)
{
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return nullptr;
	}
	
	if (!Npc)
	{
		UE_LOG(SkAnimation, Warning, TEXT("!Npc"));
		return nullptr;
	}

	auto AnimationData = GameInstance->AssetManager->GetAnimationV2(Npc->Visual, AnimationName);
	if (!AnimationData)
	{
		UE_LOG(SkAnimation, Warning, TEXT("!AnimationData"));
		return nullptr;
	}

	if (bRootMotion)
	{
		return AnimationData->AnimMontage;
	}
	else
	{
		if (AnimationData->AnimMontageNoRootMotion)
			return AnimationData->AnimMontageNoRootMotion;

		if (!AnimationData->AnimSequence)
			return nullptr;
		
		AnimationData->AnimSequenceNoRootMotion = DuplicateObject<UAnimSequence>(AnimationData->AnimSequence,this);
		if (!AnimationData->AnimSequenceNoRootMotion)
			return nullptr;

		AnimationData->AnimSequenceNoRootMotion->bEnableRootMotion = false;
		AnimationData->AnimSequenceNoRootMotion->bForceRootLock = true;
		
		AnimationData->AnimMontageNoRootMotion = CreateAnimMontageFromAnimSequence(
			AnimationData->AnimSequenceNoRootMotion, AnimationData);
		if (!AnimationData->AnimMontageNoRootMotion)
			return nullptr;

		AnimationData->AnimMontageNoRootMotion->Rename(*(AnimationData->Name + "_NO_ROOT_MOTION"));
	}
	
	return AnimationData->AnimMontageNoRootMotion;
}

UAnimationData* UGHAnimInstance::GetAnimationDataByName(const FString& AnimationName)
{
	// auto EmptyAnimationData = FAnimationData();

	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return nullptr;
	}
	
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	// if (!GameMode || !GameMode->AssetManager)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!!GameMode || !GameMode->AssetManager"));
 //        return EmptyAnimationData;
	// }

	if (!Npc)
	{
		UE_LOG(SkAnimation, Warning, TEXT("!Npc"));
		return nullptr;
	}

	auto AnimationData = GameInstance->AssetManager->GetAnimationV2(Npc->Visual, AnimationName);
	if (!AnimationData)
	{
		UE_LOG(SkAnimation, Warning, TEXT("!AnimationData"));
		// cant returl link to local var
		return nullptr;
	}
	
	return AnimationData;
	
	// auto EmptyAnimationData = FAnimationData();
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	// if (!GameInstance)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!GameInstance"));
	// 	return EmptyAnimationData;
	// }
	//
	// if (!Npc)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!Npc"));
	// 	return EmptyAnimationData;
	// }
	//
	// auto SharedAnimationGroupData = GameInstance->SharedAnimationGroupDataMap.Find(Npc->Visual);
	// if (!SharedAnimationGroupData)
	// {
	// 	// UE_LOG(SkAnimation, Warning, TEXT("!SharedAnimationGroupData"));
	// 	return EmptyAnimationData;
	// }
	//
	// auto AnimationData = SharedAnimationGroupData->AnimationDataMap.Find(AnimationName);
	// if (!AnimationData)
	// {
	// 	// UE_LOG(SkAnimation, Warning, TEXT("!AnimationData"));
	// 	return EmptyAnimationData;
	// }
	//
	// return *AnimationData;
}

bool UGHAnimInstance::IsMontagePlayed(const FString& AnimationName)
{
	for (auto& CurrentAnimationDataLayerTuple : CurrentAnimationDataLayerMap)
	{
		if (CurrentAnimationDataLayerTuple.Value->Name.ToLower() == AnimationName.ToLower())
		{
			return true;
		}
	}
	
	return false;
}

void UGHAnimInstance::StopAllMontagesByLayer(const FString& Layer)
{
	for (int32 InstanceIndex = 0; InstanceIndex < MontageInstances.Num(); InstanceIndex++)
	{
		auto MontageInstance = MontageInstances[InstanceIndex];
		if (!MontageInstance) //  || !MontageInstance->IsActive()
			continue;
		
		auto AnimationData = CurrentAnimationDataLayerMap.FindRef(Layer);
		if (!AnimationData || !AnimationData->AnimMontage)
			continue;
		
		auto Montage = MontageInstance->Montage;
		// if (AnimationData->AnimMontage == Montage || AnimationData->AnimMontageNoRootMotion == Montage)
		if (AnimationData->AnimMontage != Montage)
			continue;

		// auto MontageBlendSettings = FMontageBlendSettings(0.0f);
		// MontageBlendSettings.Blend = AnimationData->BlendOut;
		MontageInstance->Stop(FAlphaBlend(0.1f));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			"MontageInstance->Stop: " + AnimationData->Name + ", blendouttime " + FString::SanitizeFloat(AnimationData->BlendOut));
	}
}

void UGHAnimInstance::StopMontage(UAnimationData* AnimationData, float BlendOut)
{

	// Use BlendOut = 0.001 for "instant" stop
	
	for (int32 InstanceIndex = 0; InstanceIndex < MontageInstances.Num(); InstanceIndex++)
	{
		auto MontageInstance = MontageInstances[InstanceIndex];
		if (!MontageInstance) //  || !MontageInstance->IsActive()
			continue;
		
		auto Montage = MontageInstance->Montage;
		if (AnimationData->AnimMontage == Montage || AnimationData->AnimMontageNoRootMotion == Montage)
		{
			// auto MontageBlendSettings = FMontageBlendSettings(0.0f);
			// MontageBlendSettings.Blend = AnimationData->BlendOut;
			if (BlendOut == 0.0f)
				BlendOut = AnimationData->BlendOut;
			MontageInstance->Stop(FAlphaBlend(BlendOut));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			// 	"MontageInstance->Stop: " + AnimationData->Name + ", blendouttime " + FString::SanitizeFloat(AnimationData->BlendOut));

			// Try turn off all this anim...
			// return;
		}
	}
}

float UGHAnimInstance::PlayMontage(UAnimationData* AnimationData, bool bRootMotion, bool bPlaySame, bool bPlayTransitionInstead)
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "PlayMontage: " + AnimationData.Name);

	if (!AnimationData)
		return 0.0f;
	
	if (AnimationData->Name.IsEmpty() || !AnimationData->AnimSequence)
		return 0.0f;

	auto CurrentLayerAnimationData = CurrentAnimationDataLayerMap.FindRef(AnimationData->Layer);
	if (CurrentLayerAnimationData)
	{
		if (!bPlaySame)
		{
			if (CurrentLayerAnimationData == AnimationData)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "DONT PLAY THE SAME anim: " + AnimationData.Name);
				return 0.0f;
			}
		}
	}
	// else
	// {
	// 	if (CurrentAnimationDataLayerMap[AnimationData->Layer] == AnimationData)
	// 	{
	// 		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "PPPPLAY THE SAME anim: " + AnimationData.Name);
	// 	}
	// }

	auto ChangedAnimation = CheckHardCodeAnimation(AnimationData->Name);
	if (ChangedAnimation != AnimationData->Name)
		return PlayMontageByName(ChangedAnimation, bRootMotion, bPlaySame, bPlayTransitionInstead);

	if (bRootMotion)
	{
		if (!AnimationData->AnimMontage)
		{
			if (!SetupAnimationData(AnimationData, bRootMotion))
				return 0.0f;
			// AddAnimNotifyToAnimMontage(AnimationData.AnimMontage);
		}
	}
	else
	{
		if (!AnimationData->AnimMontageNoRootMotion)
		{
			if (!SetupAnimationData(AnimationData, bRootMotion))
				return 0.0f;
			// AddAnimNotifyToAnimMontage(AnimationData.AnimMontageNoRootMotion);
		}
	}

	if (CurrentLayerAnimationData)
	{
		if (bPlayTransitionInstead)
		{
			auto TransitionAnimationName = ComputeTransitionAnimation(
				CurrentLayerAnimationData->Name,AnimationData->Name);
			if (!TransitionAnimationName.IsEmpty())
			{
				return PlayMontageByName(TransitionAnimationName, bRootMotion, bPlaySame, false);
				// // TODO: in walk mode spam forward and backward = crush string.h::109
				// TransitionAnimationName = CheckHardCodeAnimation(TransitionAnimationName);
				// auto TransitionAnimationData = GetAnimationDataByName(TransitionAnimationName);
				// if (TransitionAnimationData.IsValid())
				// 	return PlayMontage(TransitionAnimationData, bRootMotion, false, false);
			}
		}
	}

	auto AnimMontage = bRootMotion ? AnimationData->AnimMontage : AnimationData->AnimMontageNoRootMotion;
	if (!AnimMontage)
		return 0.0f;

	// if (AnimationData->Name == "T_RUNTURNL")
	// {
	// 	// AnimationData->AnimMontage->BlendIn
	// }

	// StopAllMontagesByLayer(AnimationData->Layer);
	auto MontageLenght = Montage_Play(AnimMontage, 1.0f, EMontagePlayReturnType::MontageLength,
		0.0f, true);
	
	check(MontageLenght > 0.0f);

	auto Layer = AnimationData->Layer;
	// PreviousAnimationDataLayerMap[Layer] = CurrentAnimationDataLayerMap[Layer];
	CurrentAnimationDataLayerMap[Layer] = AnimationData;
	AnimationHistoryAdd(AnimationData->Name, bRootMotion);
	
	// 
	// if (AnimationData != CurrentAnimationDataLayerMap[Layer])
	// {
	// 	PreviousAnimationDataLayerMap[Layer] = CurrentAnimationDataLayerMap[Layer];
	// 	CurrentAnimationDataLayerMap[Layer] = AnimationData;
	// 	AnimationStartTime = FDateTime::Now();
	// 	
	// 	AnimationHistorySet(AnimationData->Name, bRootMotion);
	// }
	// else
	// {
	// 	// for the event
	// 	CurrentAnimationDataLayerMap[AnimationData->Layer] = AnimationData;
	// 	
	// 	// remove new "No Animation"
	// 	AnimationHistoryRemove();
	// 	// GameMode->Debug->AnimationEndUpdate(-1.0f, false);
	// 	//AnimationHistoryUpdate();
	// }

	// auto NotifyEntry = FString::Printf(TEXT("play anim, events count: %i"), AnimationData.Events.Num());
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, NotifyEntry);
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, NotifyEntry);
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	// "Montage_Play anim: " + AnimationData.Name
	// + " layer: " + AnimationData.Layer
	// + ", len: " + FString::SanitizeFloat(MontageLenght));
	
	return MontageLenght;
}

float UGHAnimInstance::PlayMontageByName(const FString& AnimationName, bool bRootMotion, bool bPlaySame, bool bPlayTransitionInstead)
{
	auto AnimationData = GetAnimationDataByName(AnimationName);
	if (!AnimationData || !AnimationData->IsValid())
	{
		UE_LOG(SkAnimation, Warning, TEXT("Can't find AnimationData by name: %s"), *AnimationName);
		return 0.0f;
	}
	
	return PlayMontage(AnimationData, bRootMotion, bPlaySame, bPlayTransitionInstead);
}

bool UGHAnimInstance::PlayMontageQueue(TArray<FString> AnimationNameArray)
{
	if (AnimationNameArray.Num() > 0)
	{
		AnimationDataQueueArray.Empty();
		
		for (auto AnimationName : AnimationNameArray)
		{
			auto AnimationData = GetAnimationDataByName(AnimationName);
			if (!AnimationData)
			{
				UE_LOG(SkAnimation, Warning, TEXT("Can't find AnimationData by name: %s"), *AnimationName);
				AnimationDataQueueArray.Empty();
				return false;
			}

			AnimationDataQueueArray.Add(AnimationData);
		}

		PlayMontage(AnimationDataQueueArray[0]);
	}

	return true;
}

void UGHAnimInstance::PlayMontageByNameOld(const FString& MontageName)
{
	// if (MontageName == CurrentAnimationName)
	// {
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "cancel anim (same name) " + CurrentAnimationName + " => " + MontageName);
	// 	return;
	// }
	//
	// // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Try start anim: " + MontageName);
	//
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	// if (!GameInstance)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!GameInstance"));
	// 	return;
	// }
	//
	// if (!Npc)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!Npc"));
	// 	return;
	// }
	//
	// auto SharedAnimationGroupData = GameInstance->SharedAnimationGroupDataMap.Find(Npc->Visual);
	// if (!SharedAnimationGroupData)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!SharedAnimationGroupData"));
	// 	return;
	// }
	//
	// // Pointer valid till map change
	// FAnimationData* AnimationData = nullptr;
	// auto TransitionAnimationName = ComputeTransitionAnimation(MontageName);
	// if (!TransitionAnimationName.IsEmpty())
	// 	AnimationData = SharedAnimationGroupData->AnimationDataMap.Find(TransitionAnimationName);
	//
	// if (AnimationData && AnimationData->NextAnimation == MontageName && AnimationData->AnimMontage)
	// {
	// 	// all ok
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Try start TRANSITION anim: " + TransitionAnimationName);
	// }
	// else
	// {
	// 	AnimationData = SharedAnimationGroupData->AnimationDataMap.Find(MontageName);
	// 	if (!AnimationData)
	// 	{
	// 		ResetStateMachine();
	// 		UE_LOG(SkAnimation, Warning, TEXT("!AnimationData"));
	// 		return;
	// 	}
	// }
	//
	// if (!AnimationData->AnimMontage)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!AnimationData->AnimMontage"));
	// 	return;
	// }
	//
	// // auto AnimationLenght = AnimationData->EndTime - AnimationData->StartTime;
	// // auto AnimationTotalBlend = AnimationData->BlendIn + AnimationData->BlendOut;
	// // AnimationRemainSecondsWithBlend = (AnimationLenght / 1000.f) * (1.0f - AnimationData->BlendOut); // clamp 0 1
	//
	// // Montage_Stop(40.0f, nullptr);
	// // auto Lenght = Montage_Play(AnimationData->AnimMontage);
	// auto BlendInSettings = FAlphaBlendArgs(AnimationData->BlendIn);
	// auto Lenght = Montage_PlayWithBlendIn(AnimationData->AnimMontage, BlendInSettings, 1.0f,
	// 	EMontagePlayReturnType::MontageLength, (AnimationData->StartTime / 1000.0f) + 0.001f,
	// 	true);
	// if (Lenght > 0.0f)
	// {
	// 	CurrentMontage = AnimationData->AnimMontage;
	// 	CurrentAnimationName = AnimationData->Name;
	// 	NextAnimationName = AnimationData->NextAnimation;
	// 		
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "anim lenght: " + FString::SanitizeFloat(Lenght, 3));
	// }
}

void UGHAnimInstance::PlayMontageTest2()
{
	// TObjectPtr<UAnimSequence> AnimSeqToPlay;
	//
	// FString MontageName = "T_JUMPB";
	//
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	// if (!GameInstance)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!GameInstance"));
	// 	return;
	// }
	//
	// if (!Npc)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!Npc"));
	// 	return;
	// }
	//
	// auto SharedAnimationGroupData = GameInstance->SharedAnimationGroupDataMap.Find(Npc->Visual);
	// if (!SharedAnimationGroupData)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!SharedAnimationGroupData"));
	// 	return;
	// }
	//
	// // Pointer valid till map change
	// auto AnimationData = SharedAnimationGroupData->AnimationDataMap.Find(MontageName);
	// if (!AnimationData)
	// {
	// 	ResetStateMachine();
	// 	UE_LOG(SkAnimation, Warning, TEXT("!AnimationData"));
	// 	return;
	// }
	//
	// if (!AnimationData->AnimMontage)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!AnimationData->AnimMontage"));
	// 	return;
	// }
	// // auto SkelMesh = UGHGLTFRuntime::LoadSkeletalMesh("C:/archolos/test/HUM_BODY_NAKED0.gltf", nullptr);
}

void UGHAnimInstance::PlayMontageTest()
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return;
	
	if (!GameMode)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "PlayMontageTest OK 0");

	// WORK !!!!!!!!!!!!!!
	// PlayMontageByName("S_1H", true, true, false);

	PlayMontageQueue({"T_RUN_2_1H", "S_1H", "T_1H_2_1HRUN"});
	
	return;




	

	// AGHNPC* Diego = nullptr;
	// for (auto NpcTemp : GameMode->NpcArray)
	// {
	// 	auto Name = NpcTemp->_table.GetField("name").ToString();
	// 	if (Name == "Diego")
	// 	{
	// 		Diego = NpcTemp;
	// 		break;
	// 	}
	// }
	//
	// if (!Diego)
	// 	return;
	//
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "PlayMontageTest OK 1");
	//
	// auto AminSeqTemp = GetAnimSequenceByName(IdleAnimationData.Name);
	// if (AminSeqTemp)
	// {	
	// 	Diego->AnimInstance->AnimSequenceIdle = DuplicateObject<UAnimSequence>(AminSeqTemp, this, "S_RUN_NO_ROOTMOTION");
	// 	if (Diego->AnimInstance->AnimSequenceIdle)
	// 	{
	// 		Diego->AnimInstance->AnimSequenceIdle->bEnableRootMotion = false;
	// 		Diego->AnimInstance->AnimSequenceIdle->bForceRootLock = true;
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "DuplicateObject OK");
	// 	}
	// 	else
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "DuplicateObject ERR");
	// 	}
	// }
	// else
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AminSeqTemp ERR");
	// }
	//
	// AminSeqTemp = GetAnimSequenceByName(MoveForwardAnimationData.Name);
	// if (AminSeqTemp)
	// {	
	// 	Diego->AnimInstance->AnimSequenceMoveForward = DuplicateObject<UAnimSequence>(AminSeqTemp, this, "S_RUNL_NO_ROOTMOTION");
	// 	if (Diego->AnimInstance->AnimSequenceMoveForward)
	// 	{
	// 		Diego->AnimInstance->AnimSequenceMoveForward->bEnableRootMotion = false;
	// 		Diego->AnimInstance->AnimSequenceMoveForward->bForceRootLock = true;
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "DuplicateObject OK");
	// 	}
	// 	else
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "DuplicateObject ERR");
	// 	}
	// }
	// else
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AminSeqTemp ERR");
	// }
	
	// AnimSequenceIdle = GetAnimSequenceByName(IdleAnimationName);
	// AnimSequenceMoveForward = GetAnimSequenceByName(MoveForwardAnimationName);
	//
	// if (AnimSequenceMoveForward)
	// {
	// 	AnimSequenceMoveForward->bForceRootLock = true;
	// }
	
	// return;
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	// if (!GameInstance)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GameInstance NOT OK");
	// 	UE_LOG(SkAnimation, Warning, TEXT("!GameInstance"));
	// 	return;
	// }
	// else
	// {
	// 	if (GameInstance->SharedAnimationGroupDataMap.Num() > 0)
	// 	{
	// 		auto AnimationGroupData = GameInstance->SharedAnimationGroupDataMap.Find("HUMANS");
	// 		if (AnimationGroupData && AnimationGroupData->AnimationDataMap.Num() > 0)
	// 		{
	// 			auto AnimationData = AnimationGroupData->AnimationDataMap.Find("S_RUN");
	// 			if (AnimationData)
	// 			{
	// 				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Found anim asc: " + AnimationData->AscName);
	// 				
	// 				if (!CurrentSkeleton)
	// 					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "!CurrentSkeleton");
	//
	// 				if (!AnimationData->AnimMontage->GetSkeleton())
	// 					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "!AnimationData->AnimMontage->GetSkeleton()");
	//
	// 				auto MontageLenght = Montage_Play(AnimationData->AnimMontage);
	// 			}
	// 		}
	// 	}





	
		
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GameInstance OK");
		//
		// auto Lenght = FString::FromInt(GameInstance->SharedAnimationGroupDataMap.Num());
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Lenght: " + Lenght);
		//
		// if (GameInstance->SharedAnimationGroupDataMap.Num() > 0)
		// {
		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Num > 0");
		//
		// 	for (auto SharedAnimationGroupDataTuple : GameInstance->SharedAnimationGroupDataMap)
		// 	{
		// 		auto GroupName = SharedAnimationGroupDataTuple.Key;
		// 		if (GroupName == AnimationGroupName)
		// 		{
		// 			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "FOUND HUMANS");
		// 		}
		// 	}
		// }



	
	//
	// 	auto AnimationGroupData = GameInstance->SharedAnimationGroupDataMap.Find(AnimationGroupName);
	// 	if (AnimationGroupData)
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimationGroupData OK");
	// 	}
	// 	else
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimationGroupData NOT OK");
	// 	}
	// 	
	// 	return;
	// }
	//
	// if (!GameInstance->SharedAnimationGroupDataMap.Contains(AnimationGroupName))
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("!GameInstance->SharedAnimationGroupDataMap.Contains(Visual)"));
	// 	return;
	// }
	//
	// if (!GameInstance->SharedAnimationGroupDataMap[AnimationGroupName].AnimationDataMap.Contains("S_RUN"))
	// {
	// 	UE_LOG(SkAnimation, Warning,
	// 		TEXT("!GameInstance->SharedAnimationGroupDataMap[Visual].AnimationDataMap.Contains('S_RUN')"));
	// 	return;
	// }
	//
	// auto AnimMontage = GameInstance->SharedAnimationGroupDataMap[AnimationGroupName].AnimationDataMap["S_RUN"].AnimMontage;
	// if (!AnimMontage)
	// {
	// 	UE_LOG(SkAnimation, Warning, TEXT("AnimMontage"));
	// 	return;
	// }
	//
	// auto MontageLenght = Montage_Play(AnimMontage, 1);
	// Montage_JumpToSection(FName("S_RUN"), AnimMontage);
	// FMontageBlendSettings BlendInSettings(GameInstance->SharedAnimationGroupDataMap[Visual].AnimationDataMap["S_RUN"].BlendIn);
	// FMontageBlendSettings BlendOutSettings(GameInstance->SharedAnimationGroupDataMap[Visual].AnimationDataMap["S_RUN"].BlendOut);
	// float Montage_PlayWithBlendSettings(AnimMontage, BlendInSettings, 1.f, h, float InTimeToStartMontageAt=0.f, bool bStopAllMontages = true);

	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "start PlayMontageTest");

	auto MeshAndAnimation = "T_JUMPB.glb";
	auto PathToModel = "C:/archolos/test/HUM_BODY_NAKED0.glb";
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	
	// auto RuntimeAssetSkeletonAndMesh = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToModel,false, LoaderConfig);
	// if (!RuntimeAssetSkeletonAndMesh)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "RuntimeAssetSkeletonAndMesh ERR");
	// 	return;
	// }
	//
	// FglTFRuntimeSkeletalMeshConfig RuntimeSkeletalMeshConfig;
	// // RuntimeSkeletalMeshConfig.Skeleton = SkeletonTest;
	// // RuntimeSkeletalMeshConfig.bOverwriteRefSkeleton = true;
	// auto LoadedSkeletalMesh = RuntimeAssetSkeletonAndMesh->LoadSkeletalMesh(0, 0, RuntimeSkeletalMeshConfig);
	// if (!LoadedSkeletalMesh)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LoadedSkeletalMesh ERR");
	// 	return;
	// }
	
	// CurrentSkeleton = LoadedSkeletalMesh->GetSkeleton();
	
	// auto SkeletalMeshComp = GetSkelMeshComponent();
	// if (!SkeletalMeshComp)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SkeletalMeshComp ERR");
	// 	return;
	// }
	// my
	// SkeletalMeshComp->SetSkeletalMesh(LoadedSkeletalMesh);


	
	// // auto PathToFileAnim = "C:/archolos/test/HUM_RUNLOOP_M01.gltf";
	// auto PathToFileAnim = "C:/archolos/animations/gltf/HUM_RUNLOOP_M01.glb";
	// auto RuntimeAssetAnimation = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFileAnim,false, LoaderConfig);
	// if (!RuntimeAssetAnimation)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "RuntimeAssetAnimation ERR");
	// 	return;
	// }
	//
	// FglTFRuntimeSkeletalAnimationConfig RSAConfig;
	// auto AnimSequence = RuntimeAssetAnimation->LoadSkeletalAnimation(SkeletalMesh, 0, RSAConfig);
	// if (!AnimSequence)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Anim ERR");
	// 	return;
	// }


	auto SkMesh = UGHGLTFRuntime::LoadSkeletalMesh("HUM_BODY_NAKED0", nullptr);

	
	auto SkeletalMeshComp = GetSkelMeshComponent();
	if (!SkeletalMeshComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SkeletalMeshComp ERR");
		return;
	}
	auto AnimSequence = UGHGLTFRuntime::LoadAnimSequence(SkMesh/*SkeletalMeshComp->GetSkeletalMeshAsset()*/, "HUM_RUNAMBIENT_M01");
	if (!AnimSequence)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimSequence ERR");
		return;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimSequence OK");
		AnimSequenceIdle = AnimSequence;
	}

	return;

	// TObjectPtr<UAnimSequence> AnimSeqToPlay;
	
	FMontageBlendSettings BlendInSettings(0.0f);
	FMontageBlendSettings BlendOutSettings(0.0f);
	auto AnimMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage_WithBlendSettings(AnimSequence,
		"DefaultSlot", BlendInSettings, BlendOutSettings, 1, 1, -1);
	if (!AnimMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimMontage ERR");
		return;
	}

	AnimMontageIdle = AnimMontage;

	// AnimMontageAddSection(AnimMontage, TEXT("T_RUN_2_1H"), 0.0f);
	// AnimMontageAddSection(AnimMontage, TEXT("S_1H"), 0.13f - 0.001f);
	// AnimMontageAddSection(AnimMontage, TEXT("T_1H_2_1HRUN"), 0.131f + 0.001f);
	//Montage_SetPlayRate(AnimMontage, 0.1);
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Montage_Play ->");
	// auto MontageLenght = Montage_Play(AnimMontage);
	// auto MLen = FString::SanitizeFloat(MontageLenght);
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "MontageLenght: " + MLen);

	
	//Montage_SetPlayRate(AnimMontage, 0.1);
	//Montage_JumpToSection(FName("T_1H_2_1HRUN"), AnimMontage);
	//auto Montage = PlaySlotAnimationAsDynamicMontage(AnimationTest, "DefaultSlot", 0.25, 0.25, 1, 1, -1, 0);
	
	// if (AnimationTest)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "PlayMontageTest");
	// 	
	// 	FMontageBlendSettings BlendInSettings(0.0f);
	// 	FMontageBlendSettings BlendOutSettings(0.0f);
	// 	auto Montage = UAnimMontage::CreateSlotAnimationAsDynamicMontage_WithBlendSettings(AnimationTest,
	// 		"DefaultSlot", BlendInSettings, BlendOutSettings, 1, 1, -1);
	// 	auto MontageLenght = Montage_Play(Montage);
	// 	//auto Montage = PlaySlotAnimationAsDynamicMontage(AnimationTest, "DefaultSlot", 0.25, 0.25, 1, 1, -1, 0);
	// }
}

void UGHAnimInstance::StopAllMontage(float InBlendOutTime)
{
	Montage_Stop(InBlendOutTime, nullptr);
}


void UGHAnimInstance::ChooseMoveDirection(bool bPlaySame)
{
	// if (bIsMoveLeftPressed && !bIsMoveRightPressed)
	// {
	// 	PlayMontage(MoveLeftAnimationData, true, true, false);
	// }
	// else if (!bIsMoveLeftPressed && bIsMoveRightPressed)
	// {
	// 	PlayMontage(MoveRightAnimationData, true, true, false);
	// }
	// else if ((bIsMoveLeftPressed && bIsMoveRightPressed) || (!bIsMoveLeftPressed && !bIsMoveRightPressed))
	// {
	// 	PlayMontage(IdleAnimationData, true, true, false);
	// }
}

void UGHAnimInstance::StartMoveForward()
{
	bIsMoveForwardPressed = true;
	UpdateStateMachine();
}

void UGHAnimInstance::StopMoveForward()
{
	bIsMoveForwardPressed = false;
	UpdateStateMachine();
}

void UGHAnimInstance::StartMoveBackward()
{
	bIsMoveBackwardPressed = true;
	UpdateStateMachine();
}

void UGHAnimInstance::StopMoveBackward()
{
	bIsMoveBackwardPressed = false;
	UpdateStateMachine();
}

void UGHAnimInstance::StartMoveLeft()
{
	bIsMoveLeftPressed = true;
	UpdateStateMachine();
}

void UGHAnimInstance::StopMoveLeft()
{
	bIsMoveLeftPressed = false;
	UpdateStateMachine();
}

void UGHAnimInstance::StartMoveRight()
{
	bIsMoveRightPressed = true;
	UpdateStateMachine();
}

void UGHAnimInstance::StopMoveRight()
{
	bIsMoveRightPressed = false;
	UpdateStateMachine();
}

void UGHAnimInstance::ChooseRotateDirection()
{
	// if (bIsTurnLeftPressed && !bIsTurnRightPressed)
	// {
	// 	PlayMontage(TurnLeftAnimationData, false, false, false);
	// }
	// else if (!bIsTurnLeftPressed && bIsTurnRightPressed)
	// {
	// 	PlayMontage(TurnRightAnimationData, false, false, false);
	// }
	// else
	// {
	// 	// Stop becose it's not main layer
	// 	StopMontage(TurnLeftAnimationData);
	// 	StopMontage(TurnRightAnimationData);
	// }
	
	//
	// // add turn animation when turn, add only when stay/idle
	// if (true)//(CurrentAnimationDataLayerMap["1"] == IdleAnimationData && IdleAnimationData->Name.Len() > 0)
	// {
	// 	//PlayMontage(TurnLeftAnimationData, false, true, false);
	// 	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "PlayMontage TurnLeftAnimationData");
	// 	
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "CurrentAnimationDataLayerMap[1] == IdleAnimationData");
	// 	
	// 	// we can turn from mouse or buttons, mouse turn have min speed
	// 	if (bIsTurnLeftPressed && !bIsTurnRightPressed)
	// 	{
	// 		PlayMontage(TurnLeftAnimationData, false, false, false);
	// 	}
	// 	else if (bIsTurnRightPressed && !bIsTurnLeftPressed)
	// 	{
	// 		PlayMontage(TurnRightAnimationData, false, false, false);
	// 	}
	// 	// else if (IsMontagePlayed("T_RUNTURNL") || IsMontagePlayed("T_RUNTURNR"))
	// 	// {
	// 	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "NO TURN BUTTONS");
	// 	// 	// ???
	// 	// 	// Hardcode blendout
	// 	// 	auto AnimMontage = CurrentAnimationDataLayerMap["20"]->AnimMontageNoRootMotion;
	// 	// 	if (AnimMontage)
	// 	// 	{
	// 	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AnimMontage 20 ok");
	// 	// 		
	// 	// 		Montage_Stop(0.2, AnimMontage);
	// 	//
	// 	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "STOP TURN");
	// 	// 	}
	// 	// }
	// }
}

void UGHAnimInstance::StartTurnLeft()
{
	bIsTurnLeftPressed = true;

	if (Npc)
		Npc->DebugString1 = "bIsTurn_Left_Pressed: true";

	UpdateStateMachine();
}

void UGHAnimInstance::StopTurnLeft()
{
	bIsTurnLeftPressed = false;

	if (Npc)
		Npc->DebugString1 = "bIsTurn_Left_Pressed: false";

	UpdateStateMachine();

	// StopMontage(TurnLeftAnimationData);

	// auto AnimationDataLayer20 = CurrentAnimationDataLayerMap.FindRef("20");
	// if (AnimationDataLayer20 && AnimationDataLayer20 == TurnLeftAnimationData)
	// {
	// 	StopMontage(TurnLeftAnimationData);
	// }

	// // add turn animation when turn, add only when stay/idle
	// if (CurrentAnimationDataLayerMap["1"] == IdleAnimationData && IdleAnimationData->Name.Len() > 0)
	// {
	// 	if (CurrentAnimationDataLayerMap["20"] == TurnLeftAnimationData && TurnLeftAnimationData->Name.Len() > 0)
	// 	{
	// 		StopMontage(TurnLeftAnimationData);
	// 		// auto BlendOut = CurrentAnimationDataLayerMap["20"].BlendOut;
	// 		// auto AnimMontage = CurrentAnimationDataLayerMap["20"].AnimMontageNoRootMotion;
	// 		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "BlendOut: " + FString::SanitizeFloat(TurnLeftAnimationData.BlendOut));
	// 		// CurrentAnimationDataLayerMap["20"] = nullptr;
	// 		// Montage_Stop(0.9, TurnLeftAnimationData.AnimMontageNoRootMotion);
	// 		// FAnimMontageInstance* MontageInstance = GetActiveInstanceForMontage(Montage);
	// 		// if (MontageInstance)
	// 		// {
	// 		// 	MontageInstance->Stop(AlphaBlendSelectorFunction(MontageInstance));
	// 		// }
	// 	}
	// }
}

void UGHAnimInstance::StartTurnRight()
{
	bIsTurnRightPressed = true;

	if (Npc)
		Npc->DebugString1 = "bIsTurn_Right_Pressed: true";

	UpdateStateMachine();
}

void UGHAnimInstance::StopTurnRight()
{
	bIsTurnRightPressed = false;

	if (Npc)
		Npc->DebugString1 = "bIsTurn_Right_Pressed: false";

	UpdateStateMachine();
}
