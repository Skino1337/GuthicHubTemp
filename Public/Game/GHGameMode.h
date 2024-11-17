// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Game/Vobs/GHVobsLoader.h"

#include "Game/Vobs/GHVob.h"
#include "Game/Vobs/GHVobLight.h"
#include "Game/GHNPC.h"
#include "Game/GHDialog.h"
#include "Game/GHChapter.h"
#include "Game/GHInventory.h"
#include "Core/GHGLTFRuntime.h"
#include "Game/GHStats.h"
#include "Game/GHQuest.h"
#include "Game/GHDebug.h"
#include "Game/GHWaypoint.h"

#include "Player/GHPlayerController.h"

#include "Struct/GHQuestStruct.h"

#include "UI/GHDialogText.h"
#include "UI/GHDialogChoices.h"


#include "GHGameMode.generated.h"


// Forward Declarations
class UGHAssetManager;
class UGHDialog;
class UGHInventory;
class UGHGLTFRuntime;
class UGHStats;
class UGHQuest;
class UGHDebug;


UCLASS()
class GOTHICHUB_API AGHGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGHGameMode();
	
protected:
	virtual void BeginPlay() override;
	
public:
	/** The default pawn class used by players. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<APawn> NpcClass;

	/** The default entity class used by players. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<AActor> EntityClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<AActor> BowActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<AActor> WaypointClass;

	/** The default level class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<AActor> LevelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<AActor> VobClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<APawn> FreeCameraClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* WaterMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* WaterFallMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGHNPC*> NpcArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FWaypoint> WaypointMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVobStruct> VobArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVobLightStruct> VobLightArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FQuest> QuestMap;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, TMap<FString, TMap<FString, FString>>> LanguageMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHChapter* Chapter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHDialog* Dialog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHInventory* Inventory;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UGHAssetManager* AssetManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHGLTFRuntime* GLTFRuntime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHStats* Stats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHQuest* Quest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHDebug* Debug;

	// ---
	// Game Time
	// ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GameTimeSpeedMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GameTimeDays;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GameTimeHours;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GameTimeMinutes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GameTimeSeconds;
	
	// ---
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Axis;
	
	// ---
	// ---
	// ---
	
	UFUNCTION()
	void UpdateGameTime();
	
	UFUNCTION()
	FString GetTranslatedString(FString Data);

	UFUNCTION()
	void ShowWaypoints(bool bIsShow);

	UFUNCTION()
	void SpawnVobs();

	UFUNCTION()
	void SpawnVobLights();

	UFUNCTION()
	void Rot(int32 Side);

	UFUNCTION()
	void ChangeAxis();

	UFUNCTION()
	void LoadAnimatoins();

	UFUNCTION()
	void SpawnAllAnim();
};



