// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Core/GHLuaState.h"
#include "Game/GHNPC.h"
#include "Game/GHGameMode.h"
#include "Player/GHCharacter.h"
#include "Core/GHLuaState.h"
#include "Struct/GHAudioDataStruct.h"
#include "Struct/GHMorphDataStruct.h"
#include "Struct/GHAnimationDataStruct.h"

#include "Serialization/JsonReader.h"
#include "Serialization/JsonTypes.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"


DECLARE_LOG_CATEGORY_EXTERN(JSONParser, Log, All);


class GOTHICHUB_API GHJSON
{
public:
	UFUNCTION()
	static void LoadWaypoints();
	
	UFUNCTION()
	static void LoadLanguages();

	UFUNCTION()
	static FMorphMeshData* ParseMorphMeshData(const FString& FileData);

	UFUNCTION()
	static FAnimationGroupData* ParseAnimationGroupData(const FString& FileData);

	UFUNCTION()
	static TArray<FAudioData*> ParseAudioData(const FString& FileData);

	UFUNCTION()
	static void LoadAnimations();

	UFUNCTION()
	static void LoadSounds();
};
