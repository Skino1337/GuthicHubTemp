// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/GHAnimNotifyState.h"


void UGHAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta, "NotifyBegin");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NotifyBegin");
}

void UGHAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UGHAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
