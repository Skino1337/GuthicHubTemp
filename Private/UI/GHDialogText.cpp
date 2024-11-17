#include "UI/GHDialogText.h"

#include "Components/MultiLineEditableTextBox.h"


void UGHDialogText::UpdateText(FString Text)
{
	if (DialogTextBox)
		DialogTextBox->SetText(FText::FromString(Text));
}