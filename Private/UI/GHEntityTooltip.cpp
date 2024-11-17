// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GHEntityTooltip.h"

#include "Components/TextBlock.h"

// UGHEntityTooltip::UGHEntityTooltip()
// {
// 	
// }

void UGHEntityTooltip::NativeConstruct()
{
	// Call the Blueprint "Event Construct" node
	Super::NativeConstruct();
}

void UGHEntityTooltip::UpdateText(FString Text)
{
	if (TooltipTextBlock)
		TooltipTextBlock->SetText(FText::FromString(Text));
}