#include "..\..\Public\Animations\GHMorphedSkeletalMeshComponent.h"

#include "Engine/SkeletalMesh.h"
#include "Rendering/SkeletalMeshRenderData.h"


DEFINE_LOG_CATEGORY(MorphAnim);


UGHMorphedSkeletalMeshComponent::UGHMorphedSkeletalMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;

	CurrentMorphAnimData = FMorphAnimData();
	bPlayAnimation = false;
}

void UGHMorphedSkeletalMeshComponent::PlayAnimationByName(const FString& AnimName)
{
	// for (auto MorphDataMapTuple : MorphData.MorphDataMap)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "MorphDataMapTuple: " + MorphDataMapTuple.Value.Name);
	// }
	
	const auto MorphAnimData = MorphData.MorphAnimDataMap.Find(AnimName);
	if (!MorphAnimData)
		return;

	if (CurrentMorphAnimData.IsValid())
	{
		StopAnimation();
	}
	
	FrameCount = MorphAnimData->FrameEnd - MorphAnimData->FrameStart;
	FrameCurrent = MorphAnimData->FrameStart;
	AnimPosition = 0.001f;
	FrameWindow = 1.0f / 4; // 0.04

	CurrentMorphAnimData = *MorphAnimData;
	bPlayAnimation = true;
}

void UGHMorphedSkeletalMeshComponent::StopAnimation()
{
	if (!CurrentMorphAnimData.IsValid())
		return;

	bPlayAnimation = false;

	for (int32 Key = CurrentMorphAnimData.FrameStart; Key <= CurrentMorphAnimData.FrameEnd; Key++)
	{
		auto MorphTargetName = FName("Key " + FString::FromInt(Key));
		SetMorphTarget(MorphTargetName, 0.0f);
	}
	
	CurrentMorphAnimData = FMorphAnimData();
}

void UGHMorphedSkeletalMeshComponent::RestartAnimation()
{
	auto AnimationNameToRestart = CurrentMorphAnimData.Name;

	StopAnimation();
	PlayAnimationByName(AnimationNameToRestart);
}

void UGHMorphedSkeletalMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Tick: 1");

	// @TODO: check 1 frame anim, "L" in flags maybe loop, like "pose"
	if (!bPlayAnimation)
		return;

	if (!CurrentMorphAnimData.IsValid())
		return;

	// like "pose"
	if (CurrentMorphAnimData.FrameStart == CurrentMorphAnimData.FrameEnd)
	{
		auto MorphTargetName = FName("Key " + FString::FromInt(CurrentMorphAnimData.FrameStart));
		SetMorphTarget(MorphTargetName, 1.0f);

		return;
	}

	AnimPosition += DeltaTime;
	int32 OffsetFrameNum = AnimPosition / FrameWindow; // 0, 1, 2, ... et cetera

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AnimPosition: " + FString::SanitizeFloat(AnimPosition));
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "FrameWindow: " + FString::SanitizeFloat(FrameWindow));
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "OffsetFrameNum: " + FString::FromInt(OffsetFrameNum));
	
	if (OffsetFrameNum >= FrameCount)
	{
		if (CurrentMorphAnimData.Flags.Contains("L"))
			RestartAnimation();
		else
			StopAnimation();
		// SetMorphTarget("Key 10", 0.0f, false);
		// UE_LOG(MorphAnim, Warning, TEXT("RESET ANIM"));

		// PlayAnimationByName(CurrentMorphAnimData.Name);
		
		return;
	}

	// FrameCurrent = OffsetFrameNum + 1;

	// need add check zero divide
	double FrameTime = 0.0;
	if (AnimPosition < FrameWindow)
		FrameTime = AnimPosition; // 20 = 20
	else
		UKismetMathLibrary::FMod(AnimPosition, FrameWindow, FrameTime);
	
	// + 0.001f fix zero divide
	float EvaluatedKeyValue = 1.0f - (1.0f / (FrameWindow / (FrameTime + 0.001f)));
	float EvaluatedKeyValueNextFrame = 1.0f - EvaluatedKeyValue;

	auto MorphTargetName = FName("Key " + FString::FromInt(CurrentMorphAnimData.FrameStart + OffsetFrameNum));
	auto MorphTargetNameNextFrame = FName("Key " +
		FString::FromInt(CurrentMorphAnimData.FrameStart + OffsetFrameNum + 1));

	if (CurrentMorphAnimData.FrameStart + OffsetFrameNum - 1 >= CurrentMorphAnimData.FrameStart)
	{
		auto MorphTargetNamePrevFrame = FName("Key " +
			FString::FromInt(CurrentMorphAnimData.FrameStart + OffsetFrameNum - 1));
		SetMorphTarget(MorphTargetNamePrevFrame, 0.0f);
	}

	SetMorphTarget(MorphTargetName, EvaluatedKeyValue);
	SetMorphTarget(MorphTargetNameNextFrame, EvaluatedKeyValueNextFrame);
	
	UE_LOG(MorphAnim, Warning, TEXT("---"));
	UE_LOG(MorphAnim, Warning, TEXT("DeltaTime: %f"), DeltaTime);
	UE_LOG(MorphAnim, Warning, TEXT("AnimPosition: %f"), AnimPosition);
	UE_LOG(MorphAnim, Warning, TEXT("OffsetFrameNum: %i"), OffsetFrameNum);
	UE_LOG(MorphAnim, Warning, TEXT("FrameTime: %f"), FrameTime);
	
	if (CurrentMorphAnimData.FrameStart + OffsetFrameNum - 1 >= CurrentMorphAnimData.FrameStart)
	{
		UE_LOG(LogTemp, Warning, TEXT("EvaluatedKeyValuePrevFrame: 0.00"));
	}
	UE_LOG(MorphAnim, Warning, TEXT("EvaluatedKeyValue: %f"), EvaluatedKeyValue);
	UE_LOG(MorphAnim, Warning, TEXT("EvaluatedKeyValueNextFrame: %f"), EvaluatedKeyValueNextFrame);

	if (CurrentMorphAnimData.FrameStart + OffsetFrameNum - 1 >= CurrentMorphAnimData.FrameStart)
	{
		UE_LOG(MorphAnim, Warning, TEXT("MorphTargetNamePrevFrame: Key %s"),
			*FString::FromInt(CurrentMorphAnimData.FrameStart + OffsetFrameNum - 1));
	}
	UE_LOG(MorphAnim, Warning, TEXT("MorphTargetName: Key %s"),
		*FString::FromInt(CurrentMorphAnimData.FrameStart + OffsetFrameNum));
	UE_LOG(MorphAnim, Warning, TEXT("MorphTargetNameNextFrame: Key %s"),
		*FString::FromInt(CurrentMorphAnimData.FrameStart + OffsetFrameNum + 1));
}

void UGHMorphedSkeletalMeshComponent::SetMorphData(const FMorphMeshData& InMorphData)
{
	SetSkeletalMesh(InMorphData.SkeletalMesh);
	MorphData = InMorphData;
}