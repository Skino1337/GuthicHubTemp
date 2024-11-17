// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GHWeaponLeftBeltTouch.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API UGHWeaponLeftBeltTouch : public UAnimNotify
{
	GENERATED_BODY()

private:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
