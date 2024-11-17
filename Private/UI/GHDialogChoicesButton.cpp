// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/GHDialogChoicesButton.h"

#include "ShaderPrintParameters.h"
#include "Game/GHDialog.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void UGHDialogChoicesButton::NativeConstruct()
{
	Super::NativeConstruct();
	DialogChoicesButton->OnClicked.AddDynamic(this, &UGHDialogChoicesButton::OnClicked);
	DialogChoicesButton->OnHovered.AddDynamic(this, &UGHDialogChoicesButton::OnHovered);
	DialogChoicesButton->OnUnhovered.AddDynamic(this, &UGHDialogChoicesButton::OnUnhovered);
}

void UGHDialogChoicesButton::OnClicked()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if (IsValid(DialogClassPointer))
			DialogClassPointer->DialogStart(DialogStruct);
	});
}

void UGHDialogChoicesButton::OnHovered()
{
	auto Color = FLinearColor(0.8f, 1.0f, 0.0f, 1.0f);
	DialogChoicesButtonText->SetColorAndOpacity(Color);
}

void UGHDialogChoicesButton::OnUnhovered()
{
	auto Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	DialogChoicesButtonText->SetColorAndOpacity(Color);
}