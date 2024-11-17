// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNodes/GHAnimGraphNode_GothicBlend.h"

#include "ToolMenus.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "AnimGraphCommands.h"
#include "ScopedTransaction.h"

#include "DetailLayoutBuilder.h"
#include "Kismet2/CompilerResultsLog.h"
#include "UObject/UE5ReleaseStreamObjectVersion.h"


/////////////////////////////////////////////////////
// UGHAnimGraphNode_GothicBlend


#define LOCTEXT_NAMESPACE "A3Nodes"


// UGHAnimGraphNode_GothicBlend::UGHAnimGraphNode_GothicBlend(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// 	Node.AddPose();
// }

FText UGHAnimGraphNode_GothicBlend::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("UGHAnimGraphNode_GothicBlend_Title", "Gothic Blend");
}

FLinearColor UGHAnimGraphNode_GothicBlend::GetNodeTitleColor() const
{
	return FLinearColor(0.2f, 0.8f, 0.2f);
}

FText UGHAnimGraphNode_GothicBlend::GetTooltipText() const
{
	return LOCTEXT("UGHAnimGraphNode_GothicBlend_Tooltip", "Gothic Blend");
}

FString UGHAnimGraphNode_GothicBlend::GetNodeCategory() const
{
	return TEXT("Animation|Blends");
}

// void UGHAnimGraphNode_GothicBlend::AddPinToBlendByFilter()
// {
// 	FScopedTransaction Transaction( LOCTEXT("AddPinToBlend", "AddPinToBlendByFilter") );
// 	Modify();
//
// 	Node.AddPose();
// 	ReconstructNode();
// 	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
// }

// // Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
// #include "VictoryGame.h"
// /////////////////////////////////////////////////////
// UAnimGraphNode_VictoryTurnInPlace
// UAnimGraphNode_VictoryTurnInPlace::UAnimGraphNode_VictoryTurnInPlace(const FPostConstructInitializeProperties& PCIP) : Super(PCIP) { }
// //Title Color!
// FLinearColor UAnimGraphNode_VictoryTurnInPlace::GetNodeTitleColor() const { return FLinearColor(0,12,12,1); }
// FString UAnimGraphNode_VictoryTurnInPlace::GetNodeCategory() const { return FString("Victory Anim Nodes"); }
// FString UAnimGraphNode_VictoryTurnInPlace::GetControllerDescription() const { return TEXT("~~~ Victory Turn In Place ~~~"); }
// FString UAnimGraphNode_VictoryTurnInPlace::GetNodeTitle(ENodeTitleType::Type TitleType) const
// {
// 	FString Result = *GetControllerDescription(); Result += (TitleType == ENodeTitleType::ListView) ? TEXT("") : TEXT("\n"); return Result;
// }