// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Struct/GHAnimationDataStruct.h"

#include "UI/WelcomeScreen/GHWelcomeScreen.h"
#include "UI/Chapter/GHIntroduceChapter.h"
#include "UI/Quest/GHQuestPanel.h"
#include "Core/GHAssetManager.h"

#include "GHGameInstance.generated.h"


UCLASS()
class GOTHICHUB_API UGHGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init() override;
	
public:
	UGHGameInstance();

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TMap<FString, FAnimationData> AnimationDataMap;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TMap<FString, FAnimationGroupData> SharedAnimationGroupDataMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGHAssetManager> AssetManager;
	// UGHAssetManager* AssetManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> QuestPanelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHQuestPanel* QuestPanel;

	void LoadAnimatoins();
};
