// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ImageUtils.h"


#include "Core/GHJSON.h"
#include "Struct/GHAudioDataStruct.h"
#include "Struct/GHMorphDataStruct.h"

#include "RuntimeAudioImporterLibrary.h"
#include "glTFRuntimeFunctionLibrary.h"
#include "Struct/GHAnimationDataStruct.h"

#include "GHAssetManager.generated.h"

// // forward decl
// class UGHGameInstance;


DECLARE_LOG_CATEGORY_EXTERN(AssetManager, Log, All);


// USTRUCT(BlueprintType)
// struct FGameArrayStruct
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	TArray<FMorphData> MorphDataArray;
//
// 	FGameArrayStruct()
// 	{
// 		MorphDataArray = {};
// 	}
// };


UENUM()
enum class EAssetCategory : int32
{
	None,
	Shared,
	Game,
	Mod
};


USTRUCT(BlueprintType)
struct FTextureData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString RelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UTexture* Texture;

	FTextureData()
	{
		Name = "";
		RelativePath = "";
		Texture = nullptr;
	}

	bool IsValid()
	{
		return !Name.IsEmpty() && Texture != nullptr;
	}
};


USTRUCT(BlueprintType)
struct FUniversalMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString FileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString FileExtension;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString RelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EAssetCategory AssetCategory;

	FUniversalMeshInfo()
	{
		FileName = "";
		FileExtension = "";
		RelativePath = "";
		AssetCategory = EAssetCategory::None;
	}
};

USTRUCT(BlueprintType)
struct FStaticMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString RelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UStaticMesh* StaticMesh;

	FStaticMeshData()
	{
		Name = "";
		RelativePath = "";
		StaticMesh = nullptr;
	}

	bool IsValid()
	{
		return !Name.IsEmpty() && StaticMesh != nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSkeletalMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString RelativePath;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	// USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TMap<FString, USkeletalMesh*> SkeletalMeshMap;

	FSkeletalMeshData()
	{
		Name = "";
		RelativePath = "";
		SkeletalMeshMap = {};
	}

	bool IsValid()
	{
		return !Name.IsEmpty() && SkeletalMeshMap.Num() > 0;
	}
};


UCLASS()
class GOTHICHUB_API UGHAssetManager : public UObject
{
	GENERATED_BODY()

public:
	UGHAssetManager();

public:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UGHGameInstance* GameInstance;

	bool PRINT_LOG = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PathToFolder;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UWorld* World;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GothicHubFolderFullPath;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// class URuntimeAudioImporterLibrary* RuntimeAudioImporter;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TMap<FString, FAudioData> AudioDataMap;

	// ---
	
	TMap<FString, FUniversalMeshInfo> UniversalMeshInfoMap;

	TMap<FString, FStaticMeshData*> AllStaticMeshDataMap;
	
	TMap<FString, FTextureData> AllTextureDataMap;

	TMap<FString, FMorphMeshData*> AllMorphMeshDataMap;

	TMap<FString, FSkeletalMeshData*> AllSkeletalMeshDataMap;

	// TMap<FString, FAnimationGroupData> AllAnimationGroupDataMap;

	// UPROPERTY()
	TMap<FString, TMap<FString, FAnimationGroupData*>> AllAnimationGroupDataMapMap;

	// UPROPERTY()
	TMap<FString, FAudioData*> AllAudioDataMap;
	
	// ---
	
	// memory leak???
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, TArray<FMorphMeshData>> SharedMorphDataMap;
	// TMap<FString, FGameArrayStruct> SharedMorphDataMap;

	TMap<FString, TArray<FStaticMeshData>>  SharedStaticMeshDataMap;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, TMap<FString, FAnimationGroupData>> SharedAnimationGroupDataMapMap;

	// ---

	TArray<FMorphMeshData> GameMorphDataArray;

	TArray<FStaticMeshData> GameStaticMeshDataArray;

	TMap<FString, FAnimationGroupData> GameAnimationGroupDataMap;

	// ---

	TMap<FString, TArray<FMorphMeshData>> ModMorphDataMap;

	TMap<FString, TArray<FStaticMeshData>>  ModStaticMeshDataMap;

	TMap<FString, TMap<FString, FAnimationGroupData>> ModAnimationGroupDataMapMap;
	
	// ---

	// and "one ring to rule them all"
	// TMap<FString, FAnimationData*> AnimationDataMap;
	
	// ---
	
	UFUNCTION()
	void Initialization();

	UFUNCTION()
	void ScanTextureFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory, const FString& GameOrModKey);

	UFUNCTION()
	void ScanMeshFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory, const FString& GameOrModKey);

	UFUNCTION()
	void ScanAnimationFolderV2(const FString& FolderRelativePath, EAssetCategory AssetCategory,
		const FString& GameOrModKey);

	UFUNCTION()
	void ScanSoundFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory,
		const FString& GameOrModKey);

	UFUNCTION()
	void ScanAnimationFolder(const FString& GothicHubFolderPath, const FString& MeshesRelativePath,
		bool bIsShared, const FString& Game);

	UFUNCTION()
	void ScanGameOrModFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory,
		const FString& GameOrModName);
	
	UFUNCTION()
	void ScanAssetFolder();

	//UFUNCTION()
	bool SetupSkeletonSockets(USkeletalMesh* SkeletalMesh, const TMap<FString, FSocketData>& SocketDataMap);

	//UFUNCTION()
	bool SetupSkeletonSlots(USkeleton* Skeleton);

	//UFUNCTION()
	bool SetupAnimationGroupData(FAnimationGroupData* AnimationGroupData);

	//UFUNCTION()
	bool SetupAnimationData(UAnimationData* AnimationData, USkeletalMesh* SkeletalMesh);
	
	UFUNCTION()
	FMorphMeshData GetMorphedSkeletalMesh(const FString& RelativePath);

	// UFUNCTION()
	FMorphMeshData* GetMorphedMeshV2(const FString& RelativePath);

	// UFUNCTION()
	UAnimationData* GetAnimationFromAnimationGroupData(FAnimationGroupData* AnimationGroupData,
		const FString& RelativePath, const bool bIsRelativePath);

	UAnimationData* GetAnimationV2(const FString& GroupName, const FString& RelativePath);

	USkeletalMesh* GetVisual(const FString& GroupName);

	UFUNCTION()
	UAnimationData* GetAnimation(const FString& GroupName, const FString& RelativePath);
	
	// UFUNCTION()
	FStaticMeshData* GetStaticMesh(const FString& RelativePath);

	FStaticMeshData* GetStaticMeshV2(const FString& RelativePath);

	FSkeletalMeshData* GetSkeletalMeshV2(const FString& RelativePath);
	
	FTextureData* GetTextureData(const FString& RelativePath, int32 Variant = -1);
	
	UFUNCTION()
	FString GetFilePath(const FString& RelativePath, TArray<FString> FileFormatArray);

	UFUNCTION()
	FString GetFileFullPathV2(const FString& RelativePath, TArray<FString> FileFormatArray = {});

	// ---
	
	UFUNCTION()
	TMap<FString, UStaticMesh*> LoadWorld(const FString& WorldName);

	UFUNCTION()
	TMap<FString, USkeletalMesh*> LoadSkeletalMesh(const FString& PathToFile, USkeleton* Skeleton);
	
	UFUNCTION()
	USkeletalMesh* LoadMorphedSkeletalMesh(const FString& PathToFile);

	UFUNCTION()
	UAnimSequence* LoadAnimSequence(USkeletalMesh* SkeletalMesh, const FString& PathToFile);

	UFUNCTION()
	UStaticMesh* LoadStaticMesh(const FString& PathToFile);

	UFUNCTION()
	UObject* LoadStaticOrSkeletalMesh(const FString& PathToFile);

	// ---

	UFUNCTION()
	void LoadAllAudioInAudioDataMap();

	UFUNCTION()
	void LoadAudioInAudioDataMap(const FString& AudioName, const FString& FileName);

	// UFUNCTION()
	FAudioData* GetAudioData(const FString& RelativePath, bool bPlayAfterLoad, FVector* Location);

	// UFUNCTION()
	void LoadAudioFileAsync(const FString& PathToFile, FAudioData* AudioData, FAudioFile* AudioFile,
		bool bPlayAfterLoad, FVector* Location);
	
	// UFUNCTION()
	void PlayAudio2D(FAudioData* AudioData, FAudioFile* AudioFile);
	
	UFUNCTION()
	void PlayAudioByName2D(const FString& RelativePath);
};
