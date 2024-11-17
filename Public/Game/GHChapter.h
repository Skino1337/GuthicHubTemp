// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "UI/Chapter/GHIntroduceChapter.h"

#include "GHChapter.generated.h"


UCLASS()
class GOTHICHUB_API UGHChapter : public UObject
{
	GENERATED_BODY()
	
public:
	UGHChapter();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHIntroduceChapter* IntroduceChapter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> IntroduceChapterClass;

	UFUNCTION()
	void Initialization();

	UFUNCTION()
	void ShowScreen(FString Title, FString Subtitle, FString PathToImage, FString PathToSound, float LiveTime);

	UFUNCTION()
	void HideScreen();
};
