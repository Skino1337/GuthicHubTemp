// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AnimGraphNode_Base.h"

#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_BlendListBase.h"
#include "Animations/AnimNodes/GHAnimNode_GothicBlend.h"

#include "GHAnimGraphNode_GothicBlend.generated.h"


UCLASS(MinimalAPI)
class UGHAnimGraphNode_GothicBlend : public UAnimGraphNode_Base
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_GothicBlend Node;
	
	// virtual void AddPinToBlendByFilter();

public:

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetNodeCategory() const override;
};


// public UAnimGraphNode_Base { GENERATED_UCLASS_BODY()
// 	UPROPERTY(EditAnywhere, Category=Settings)
// 	FAnimNode_VictoryTurnInPlace Node;
// 	public:
// 	
// 	// UEdGraphNode interface
// 	virtual FString GetNodeTitle(ENodeTitleType::Type TitleType) const OVERRIDE;
// 	virtual FLinearColor GetNodeTitleColor() const OVERRIDE;
// 	virtual FString GetNodeCategory() const OVERRIDE;
// 	// End of UEdGraphNode interface
// 	protected:
// 	// UAnimGraphNode_SkeletalControlBase interface
// 	virtual FString GetControllerDescription() const;
// 	// End of UAnimGraphNode_SkeletalControlBase interface
// 	};

