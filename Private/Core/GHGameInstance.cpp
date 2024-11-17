// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GHGameInstance.h"

#include "Core/GHJSON.h"

UGHGameInstance::UGHGameInstance()
{
	// static ConstructorHelpers::FClassFinder<UUserWidget> QuestPanelClassFinder(TEXT("/Game/UI/QuestPanel"));
	// if (QuestPanelClassFinder.Class != nullptr)
	// 	QuestPanelClass = QuestPanelClassFinder.Class;
}

void UGHGameInstance::Init()
{
	Super::Init();

	AssetManager = NewObject<UGHAssetManager>(this);
	AssetManager->Initialization();
	// AssetManager->LoadAllAudioInAudioDataMap();

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHGameInstance::Init");

	// auto WelcomeScreen = CreateWidget<UGHIntroduceChapter>(GetWorld(), UGHIntroduceChapter::StaticClass());
	// if (WelcomeScreen)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "WelcomeScreen OK");
	// 	WelcomeScreen->AddToViewport();
	// }
	//
	// if (QuestPanelClass)
	// {
	// 	QuestPanel = CreateWidget<UGHQuestPanel>(GetWorld(), QuestPanelClass);
	// 	if (QuestPanel)
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "QuestPanel OK");
	// 		QuestPanel->AddToViewport();
	// 	}
	// }

	//LoadAnimatoins();
}

void UGHGameInstance::LoadAnimatoins()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHGameInstance::LoadAnimatoins");

	// // TMap<FString, FAnimationData> AnimationDataMap;
	//
	// GHJSON::LoadAnimations();
	//
	// //auto GLTFRuntime = NewObject<UGHGLTFRuntime>();
	// for (auto SharedAnimationGroupDataTuple : SharedAnimationGroupDataMap)
	// {
	// 	auto GroupName = SharedAnimationGroupDataTuple.Key;
	// 	auto AnimationDataMap = SharedAnimationGroupDataTuple.Value.AnimationDataMap;
	// 	UE_LOG(LogTemp, Log, TEXT("LoadAnimatoins: GroupName: %s"), *GroupName);
	// 	for (auto AnimationDataTuple : AnimationDataMap)
	// 	{
	// 		auto AnimationName = AnimationDataTuple.Key;
	// 		UE_LOG(LogTemp, Log, TEXT("LoadAnimatoins: AnimationName: %s"), *AnimationName);
	// 	}
	// }
}
