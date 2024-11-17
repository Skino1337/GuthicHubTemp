#include "UI/WelcomeScreen/GHWelcomeScreen.h"

void UGHWelcomeScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	auto MainCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	if (!MainCanvas)
		return;
	
	WidgetTree->RootWidget = MainCanvas;
	
	auto LoadingText = WidgetTree->ConstructWidget<UTextBlock>();
	if (!LoadingText)
		return;

	LoadingText->SetText(FText::FromString("Test"));
	MainCanvas->AddChild(LoadingText);
	
	// auto MyCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	// if(MyCanvas)
	// {
	// 	const FString tempIconPath = "Texture2D'/Game/UI/StyleBrush/FirstPersonCrosshair.FirstPersonCrosshair'";
	// 	UTexture2D* tempTexture  =  LoadObject<UTexture2D>(this,*tempIconPath);
	// 	auto CrossImage = WidgetTree->ConstructWidget<UImage>();
	// 	CrossImage->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromTexture(tempTexture));
	// 	//CrossImage->SetBrushSize(FVector2D(16.0f,16.0f));
	// 	MyCanvas->AddChild(CrossImage);
	// 	const auto tempSlot =  Cast<UCanvasPanelSlot>(CrossImage->Slot);
	// 	if(tempSlot)
	// 	{
	// 		tempSlot->SetAutoSize(true);
	// 		tempSlot->SetOffsets(FMargin(0,0));
	// 		tempSlot->SetAnchors(FAnchors(0.5f,0.5f));
	// 	}
	// 	WidgetTree->RootWidget = MyCanvas;
	// }
}

