// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHChapter.h"

#include "Game/GHGameMode.h"

#include "Components/TextBlock.h"

UGHChapter::UGHChapter()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ChapterClassFinder(TEXT("/Game/UI/IntroduceChapter"));
	if (ChapterClassFinder.Class)
		IntroduceChapterClass = ChapterClassFinder.Class;
}

void UGHChapter::Initialization()
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHChapter::Initialization");
}

void UGHChapter::ShowScreen(FString Title, FString Subtitle, FString PathToImage, FString PathToSound, float LiveTime)
{
	auto World = GEngine->GetCurrentPlayWorld();
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	if (IntroduceChapterClass)
		IntroduceChapter = CreateWidget<UGHIntroduceChapter>(World, IntroduceChapterClass);
	
	if (IntroduceChapter)
	{
		auto TranslatedTitle = GameMode->GetTranslatedString(Title);
		auto TranslatedSubtitle = GameMode->GetTranslatedString(Subtitle);

		IntroduceChapter->TextBlockTitle->SetText(FText::FromString(TranslatedTitle));
		IntroduceChapter->TextBlockSubtitle->SetText(FText::FromString(TranslatedSubtitle));

		IntroduceChapter->AddToViewport();
		
		if (LiveTime < 10.0f)
			LiveTime *= 1000.0f;

		LiveTime = FMath::Clamp(LiveTime, 2000.0f, 10000.f);

		FTimerHandle TimerHandleWidget;
		World->GetTimerManager().SetTimer(TimerHandleWidget, this, &UGHChapter::HideScreen,
			LiveTime / 1000.0f, false);
	}
}

void UGHChapter::HideScreen()
{
	if (IntroduceChapter)
	{
		IntroduceChapter->RemoveFromParent();
	}
}