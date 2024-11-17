// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GHAssetManager.h"



// // forward decl
// class UGHGameInstance;

//temp include
// #include "Components/InstancedStaticMeshComponent.h"

DEFINE_LOG_CATEGORY(AssetManager);

#define P

UGHAssetManager::UGHAssetManager()
{
	// World = nullptr;

	// hardcode correct order
	SharedStaticMeshDataMap = {};
	SharedStaticMeshDataMap.Add("Night of the Raven", {});
	SharedStaticMeshDataMap.Add("Gothic II", {});
	SharedStaticMeshDataMap.Add("Gothic", {});
	SharedStaticMeshDataMap.Add("temp", {});
	
	SharedMorphDataMap = {};
	SharedMorphDataMap.Add("Night of the Raven", {});
	SharedMorphDataMap.Add("Gothic II", {});
	SharedMorphDataMap.Add("Gothic", {});
	SharedMorphDataMap.Add("temp", {});

	SharedAnimationGroupDataMapMap = {};
	SharedAnimationGroupDataMapMap.Add("Night of the Raven", {});
	SharedAnimationGroupDataMapMap.Add("Gothic II", {});
	SharedAnimationGroupDataMapMap.Add("Gothic", {});
	SharedAnimationGroupDataMapMap.Add("temp", {});

	// temporary hardcode @TODO:
	GothicHubFolderFullPath = TEXT("C:/GothicHub");
	
	// GameAnimationGroupDataMap.Add("Archolos", {});
	
	//SharedGameSequence = {"Gothic", "Gothic II", "Night of the Raven"};
	// static ConstructorHelpers::FClassFinder<UMaterial> WaterMaterialClassFinder(TEXT("/Game/Materials/M_Water"));
	// if (WaterMaterialClassFinder.Class != nullptr)
	// 	WaterMaterialClass = WaterMaterialClassFinder.Class;
}

void UGHAssetManager::Initialization()
{
	PathToFolder = "C:/archolos/";
	
	// delegete bugs, dont change
	// World = GEngine->GetCurrentPlayWorld(nullptr);

	ScanAssetFolder();

	GetTextureData("Hum_Head_V_C0", 3);
	GetTextureData("just_tex", 3);
	GetTextureData("Hum_Head__C0", 3);
	GetTextureData("Hum_Head_C0", 3);
	GetTextureData("Hum_Head_V142_C0", 3);
	GetTextureData("Hum_Head_V2", 3);
	GetTextureData("Hum_Head_V", 3);
	GetTextureData("Hum_Head_V1.png", 3);
	GetTextureData("some_path_V1/Hum_Head_V1.png", 3);

	// // preload mesh
	// GetSkeletalMeshV2("HUM_BODY_NAKED0");
	//
	// // preload anim
	// GetAnimationV2("Humans", "S_RUN");
	//
	// PlayAudioByName2D("AIRBAG_IN");



	// auto M0 = GetStaticMeshV2("Shared/temp/Meshes/ItMw_1H_Axe_01");
	// auto M1 = GetStaticMeshV2("ItMw_1H_Axe_01");
	// auto M2 = GetStaticMeshV2("ItMw_1H_Axe_01");
	// FPaths::Combine(BaseDir, TEXT("Platforms"));

	// GetMorphedMeshV2("Shared/temp/Meshes/ITRW_BOW_H_01/ITRW_BOW_H_01");
	// GetMorphedMeshV2("ITRW_BOW_H_01");
	// GetMorphedMeshV2("ITRW_BOW_H_01");
	
	// RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	// if (RuntimeAudioImporter)
	// {
	// 	RuntimeAudioImporter->OnResultNative.AddWeakLambda(this, [this](URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status)
	// 	{
	// 		if (Status == ERuntimeImportStatus::SuccessfulImport)
	// 		{
	// 			auto Audio = UGameplayStatics::CreateSound2D(World, ImportedSoundWave);
	// 			if (Audio)
	// 			{
	// 				Audio->VolumeMultiplier = 0.07f;
	// 				Audio->bAutoDestroy = true;
	// 				Audio->Play();
	// 			}
	// 		}
	// 	});
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Failed to create runtime audio importer"));
	// }
}

void UGHAssetManager::ScanTextureFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory,
	const FString& GameOrModKey)
{
	if (GothicHubFolderFullPath.IsEmpty() || FolderRelativePath.IsEmpty() || GameOrModKey.IsEmpty())
		return;
	
	IFileManager& FileManager = IFileManager::Get();

	auto FolderFullPath = FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath);
	
	UE_LOG(AssetManager, Warning, TEXT("Find Texture folder: %s"), *FolderRelativePath);
	UE_LOG(AssetManager, Warning, TEXT("FolderFullPath: %s"), *FolderFullPath);

	// recusive
	TArray<FString> SubFolderArray;
	FileManager.FindFiles(SubFolderArray, *FPaths::Combine(FolderFullPath, TEXT("*")), false, true);
	for (auto SubFolder : SubFolderArray)
	{
		// SubFolder = folder name

		auto SubFolderRelativePath = FPaths::Combine(FolderRelativePath, SubFolder);
		
		// UE_LOG(AssetManager, Warning, TEXT("Find mesh folder 1111: %s"), *SubFolder);
		// UE_LOG(AssetManager, Warning, TEXT("SubFolderRelativePath: %s"), *SubFolderRelativePath);
		
		ScanSoundFolder(SubFolderRelativePath, AssetCategory, GameOrModKey);
	}

	// scan files in "this" dir
	TArray<FString> FilePathArray;
	FileManager.FindFiles(FilePathArray, *FPaths::Combine(FolderFullPath, TEXT("*")),
		true, false);
	for (auto& FilePath : FilePathArray)
	{
		// FilePath  = file name and file extension
		// FolderRelativePath = file relative path without file name and file extension
		
		auto FileName = FPaths::GetBaseFilename(FilePath);
		auto FileExtension = FPaths::GetExtension(FilePath);

		// UE_LOG(AssetManager, Log, TEXT("FilePath: %s"), *FilePath);

		if (FileExtension == "png")
		{
			UE_LOG(AssetManager, Warning, TEXT("find texture file: %s"), *FilePath);

			FTextureData TextureData;

			TextureData.Name = FileName;
			// With filename!!!
			TextureData.RelativePath = FPaths::Combine(FolderRelativePath, FileName);
			// TextureData.AssetCategory = AssetCategory;

			// UE_LOG(AssetManager, Warning, TEXT("FileName: %s"), *UniversalMeshInfo.FileName);
			// UE_LOG(AssetManager, Warning, TEXT("RelativePath: %s"), *UniversalMeshInfo.RelativePath);

			// RelativePath already with filename
			AllTextureDataMap.Add(TextureData.RelativePath, TextureData);
		}
	}
}

// need recursive to subfolder
void UGHAssetManager::ScanMeshFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory,
	const FString& GameOrModKey)
{
	if (GothicHubFolderFullPath.IsEmpty() || FolderRelativePath.IsEmpty() || GameOrModKey.IsEmpty())
		return;
	
	IFileManager& FileManager = IFileManager::Get();

	auto FolderFullPath = FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath);
		
	// UE_LOG(AssetManager, Warning, TEXT("Find mesh folder: %s"), *FolderRelativePath);
	// UE_LOG(AssetManager, Warning, TEXT("FolderFullPath: %s"), *FolderFullPath);

	//
	// FPaths::RemoveDuplicateSlashes()
	// FPaths::GetPath()
	
	// auto bIsShared = MeshesRelativePath.Find("Shared") == 0;
	//
	// TArray<FString> PathParts;
	// MeshesRelativePath.ParseIntoArray(PathParts, TEXT("/"), true);

	// recusive
	TArray<FString> SubFolderArray;
	FileManager.FindFiles(SubFolderArray, *FPaths::Combine(FolderFullPath, TEXT("*")), false, true);
	for (auto SubFolder : SubFolderArray)
	{
		// SubFolder = folder name

		auto SubFolderRelativePath = FPaths::Combine(FolderRelativePath, SubFolder);
		
		// UE_LOG(AssetManager, Warning, TEXT("Find mesh folder 1111: %s"), *SubFolder);
		// UE_LOG(AssetManager, Warning, TEXT("SubFolderRelativePath: %s"), *SubFolderRelativePath);
		
		ScanMeshFolder(SubFolderRelativePath, AssetCategory, GameOrModKey);
		
		
		
		// TArray<FString> MeshFolderFileArray;
		// FileManager.FindFiles(MeshFolderFileArray,
		// 	*FPaths::Combine(GothicHubFolderPath, MeshesFolderRelativePath, TEXT("*")), true, false);
		// for (auto MeshFolderFile : MeshFolderFileArray)
		// {
		// 	auto RelativePathToFile = FPaths::Combine(MeshesFolderRelativePath, MeshFolderFile);
		// 	auto FullPathToFile = FPaths::Combine(GothicHubFolderPath, RelativePathToFile);
		// 	if (FPaths::GetExtension(MeshFolderFile) == "json")
		// 	{
		// 		UE_LOG(AssetManager, Log, TEXT("find MorphMeshData file: %s"), *RelativePathToFile);
		// 		
		// 		FString JSONFileData;
		// 		if (!FFileHelper::LoadFileToString(JSONFileData, *FullPathToFile))
		// 		{
		// 			UE_LOG(AssetManager, Warning, TEXT("Can't read MorphMeshData file: %s"), *RelativePathToFile);
		// 			continue;
		// 		}
		// 		
		// 		UE_LOG(AssetManager, Log, TEXT("GetBaseFilename: %s"), *FPaths::GetBaseFilename(MeshFolderFile));
		//
		// 		auto MorphGroupData = GHJSON::ParseMorphData(JSONFileData);
		// 		if (!MorphGroupData || MorphGroupData->Name.IsEmpty())
		// 		{
		// 			UE_LOG(AssetManager, Warning, TEXT("Can't parse json data in MorphMeshData file: %s"),
		// 				*RelativePathToFile);
		// 			continue;
		// 		}
		//
		// 		MorphGroupData->RelativePath = FPaths::Combine(MeshesFolderRelativePath,
		// 			FPaths::GetBaseFilename(MeshFolderFile));
		//
		// 		// auto MeshFullPathFromMorphMeshData = GetFilePathV2(MeshRelativePathFromMorphMeshData);
		// 		// if (MeshFullPathFromMorphMeshData.IsEmpty())
		// 		// {
		// 		// 	UE_LOG(AssetManager, Warning, TEXT("Can't find file: %s"), *MeshRelativePathFromMorphMeshData);
		// 		// 	continue;
		// 		// }
		// 		// UE_LOG(AssetManager, Log, TEXT("MeshFullPathFromMorphMeshData: %s"), *MeshFullPathFromMorphMeshData);
		//
		// 		// auto MorphedSkeletalMesh = UGHGLTFRuntime::LoadMorphedSkeletalMesh(MeshFullPathFromMorphMeshData);
		// 		// if (!MorphedSkeletalMesh)
		// 		// {
		// 		// 	UE_LOG(AssetManager, Warning, TEXT("Can't load morphed skeletal mesh: %s"),
		// 		// 		*MeshRelativePathFromMorphMeshData);
		// 		// 	continue;
		// 		// }
		// 		//
		// 		// MorphGroupData->SkeletalMesh = MorphedSkeletalMesh;
		//
		// 		// add to global data
		// 		if (bIsShared)
		// 		{
		// 			if (!SharedMorphDataMap.Contains(Game))
		// 				SharedMorphDataMap.Add(Game, {});
		// 			
		// 			SharedMorphDataMap[Game].Add(*MorphGroupData);
		// 		}
		// 							
		// 		UE_LOG(AssetManager, Log, TEXT("MorphGroupData->Name: %s"), *MorphGroupData->Name);
		// 	}
		// }
	}

	// scan files in "this" dir
	TArray<FString> FilePathArray;
	FileManager.FindFiles(FilePathArray, *FPaths::Combine(FolderFullPath, TEXT("*")),
		true, false);
	for (auto& FilePath : FilePathArray)
	{
		// FilePath  = file name and file extension
		// FolderRelativePath = file relative path without file name and file extension
		
		auto FileName = FPaths::GetBaseFilename(FilePath);
		auto FileExtension = FPaths::GetExtension(FilePath);

		// UE_LOG(AssetManager, Log, TEXT("FilePath: %s"), *FilePath);
		
		if (FileExtension == "gltf" || FileExtension == "glb")
		{
			// UE_LOG(AssetManager, Warning, TEXT("find universal mesh file: %s"), *FilePath);

			FUniversalMeshInfo UniversalMeshInfo;

			UniversalMeshInfo.FileName = FileName;
			UniversalMeshInfo.FileExtension = FileExtension;
			// With filename!!!
			UniversalMeshInfo.RelativePath = FPaths::Combine(FolderRelativePath, FileName);
			UniversalMeshInfo.AssetCategory = AssetCategory;

			// UE_LOG(AssetManager, Warning, TEXT("FileName: %s"), *UniversalMeshInfo.FileName);
			// UE_LOG(AssetManager, Warning, TEXT("RelativePath: %s"), *UniversalMeshInfo.RelativePath);

			// RelativePath already with filename
			UniversalMeshInfoMap.Add(UniversalMeshInfo.RelativePath, UniversalMeshInfo);
		}
		else if (FileExtension == "json")
		{
			auto FileFullPath = FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath, FilePath);
			auto FileRelativeFullPath = FPaths::Combine(FolderRelativePath, FilePath);
			
			// UE_LOG(AssetManager, Warning, TEXT("find MorphMeshData file: %s"), *FileRelativeFullPath);
				
			FString JSONFileData;
			if (!FFileHelper::LoadFileToString(JSONFileData, *FileFullPath))
			{
				UE_LOG(AssetManager, Warning, TEXT("Can't read MorphMeshData file: %s"), *FileRelativeFullPath);
				continue;
			}
				
			// UE_LOG(AssetManager, Log, TEXT("GetBaseFilename: %s"), *FPaths::GetBaseFilename(MeshFolderFile));
			
			auto MorphMeshData = GHJSON::ParseMorphMeshData(JSONFileData);
			if (!MorphMeshData || MorphMeshData->Name.IsEmpty())
			{
				UE_LOG(AssetManager, Warning, TEXT("Can't parse json data in MorphMeshData file: %s"),
					*FileRelativeFullPath);
				continue;
			}
			
			MorphMeshData->RelativePath = FPaths::Combine(FolderRelativePath, FileName);
			
			// // add to global data
			// if (AssetCategory == EAssetCategory::Shared)
			// {
			// 	if (!SharedMorphDataMap.Contains(GameOrModKey))
			// 		SharedMorphDataMap.Add(GameOrModKey, {});
			// 		
			// 	SharedMorphDataMap[GameOrModKey].Add(*MorphMeshData);
			// }

			AllMorphMeshDataMap.Add(MorphMeshData->RelativePath, MorphMeshData);
									
			// UE_LOG(AssetManager, Log, TEXT("MorphMeshData->Name: %s"), *MorphMeshData->Name);
		}
	}
	
	// // scan static meshes
	// TArray<FString> MeshFilePathArray;
	// FileManager.FindFiles(MeshFilePathArray, *FPaths::Combine(GothicHubFolderPath, MeshesRelativePath, TEXT("*")),
	// 	true, false);
	// for (auto MeshFilePath : MeshFilePathArray)
	// {
	// 	if (FPaths::GetExtension(MeshFilePath) == "gltf" ||
	// 		FPaths::GetExtension(MeshFilePath) == "glb")
	// 	{
	// 		UE_LOG(AssetManager, Log, TEXT("find static/skeletal mesh file: %s"), *MeshFilePath);
	//
	// 		FStaticMeshData StaticMeshData;
	//
	// 		StaticMeshData.Name = FPaths::GetBaseFilename(MeshFilePath);
	// 		StaticMeshData.RelativePath = FPaths::Combine(MeshesRelativePath,
	// 			FPaths::GetBaseFilename(MeshFilePath));
	// 		
	// 		// add to global data
	// 		if (bIsShared)
	// 		{
	// 			if (!SharedStaticMeshDataMap.Contains(Game))
	// 				SharedStaticMeshDataMap.Add(Game, {});
	// 				
	// 			SharedStaticMeshDataMap[Game].Add(StaticMeshData);
	// 		}
	// 								
	// 		UE_LOG(AssetManager, Log, TEXT("add to SharedStaticMeshDataMap[%s]: %s"), *Game, *StaticMeshData.Name);
	// 	}
	// }
}

void UGHAssetManager::ScanAnimationFolderV2(const FString& FolderRelativePath, EAssetCategory AssetCategory,
	const FString& GameOrModKey)
{
	if (GothicHubFolderFullPath.IsEmpty() || FolderRelativePath.IsEmpty() || GameOrModKey.IsEmpty())
		return;
	
	IFileManager& FileManager = IFileManager::Get();

	auto FolderFullPath = FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath);
	
	// UE_LOG(AssetManager, Warning, TEXT("Find Animation folder: %s"), *FolderRelativePath);
	// UE_LOG(AssetManager, Warning, TEXT("FolderFullPath: %s"), *FolderFullPath);

	// recusive
	TArray<FString> SubFolderArray;
	FileManager.FindFiles(SubFolderArray, *FPaths::Combine(FolderFullPath, TEXT("*")), false, true);

	// sort for garantee order
	SubFolderArray.Sort();

	// reverse, "humans" in last pos
	SubFolderArray.Reserve(SubFolderArray.Num());
	
	// for (auto SubFolder : SubFolderArray)
	// {
	// 	UE_LOG(AssetManager, Warning, TEXT("anim folder order: %s"), *SubFolder);
	// }
	
	for (auto SubFolder : SubFolderArray)
	{
		// SubFolder = folder name

		auto SubFolderRelativePath = FPaths::Combine(FolderRelativePath, SubFolder);
		
		// UE_LOG(AssetManager, Warning, TEXT("Find mesh folder 1111: %s"), *SubFolder);
		// UE_LOG(AssetManager, Warning, TEXT("SubFolderRelativePath: %s"), *SubFolderRelativePath);
		
		ScanAnimationFolderV2(SubFolderRelativePath, AssetCategory, GameOrModKey);
	}

	// scan files in "this" dir
	TArray<FString> FilePathArray;
	FileManager.FindFiles(FilePathArray, *FPaths::Combine(FolderFullPath, TEXT("*")),
		true, false);
	for (auto& FilePath : FilePathArray)
	{
		// FilePath  = file name and file extension
		// FolderRelativePath = file relative path without file name and file extension
		
		auto FileName = FPaths::GetBaseFilename(FilePath);
		auto FileExtension = FPaths::GetExtension(FilePath);

		// UE_LOG(AssetManager, Log, TEXT("FilePath: %s"), *FilePath);

		if (FileExtension == "json")
		{
			auto FileFullPath = FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath, FilePath);
			auto FileRelativeFullPath = FPaths::Combine(FolderRelativePath, FilePath);
			
			// UE_LOG(AssetManager, Warning, TEXT("find AnimGroupData file: %s"), *FileRelativeFullPath);
				
			FString JSONFileData;
			if (!FFileHelper::LoadFileToString(JSONFileData, *FileFullPath))
			{
				UE_LOG(AssetManager, Warning, TEXT("Can't read AnimGroupData file: %s"), *FileRelativeFullPath);
				continue;
			}
			
			// UE_LOG(AssetManager, Log, TEXT("GetBaseFilename: %s"), *FPaths::GetBaseFilename(MeshFolderFile));
			
			auto AnimationGroupData = GHJSON::ParseAnimationGroupData(JSONFileData);
			if (!AnimationGroupData || AnimationGroupData->GroupName.IsEmpty())
			{
				UE_LOG(AssetManager, Warning, TEXT("Can't parse json data in AnimationGroupData file: %s"),
					*FileRelativeFullPath);
				continue;
			}
			
			AnimationGroupData->RelativePath = FPaths::Combine(FolderRelativePath, FileName);
			// UE_LOG(AssetManager, Log, TEXT("AnimationGroupData->RelativePath: %s"), *AnimationGroupData->RelativePath);

			// AllAnimationGroupDataMap.Add(MorphMeshData->RelativePath, MorphMeshData)

			auto BaseGroupName = AnimationGroupData->GroupName;
			auto DividerIndex = BaseGroupName.Find(TEXT("_"));
			if (DividerIndex >= 0)
			{
				BaseGroupName = BaseGroupName.Left(DividerIndex);
				if (AllAnimationGroupDataMapMap.Contains(BaseGroupName))
				{
					// @TODO: check skeleton equal
					// just add
					// if (AllAnimationGroupDataMapMap[BaseGroupName]->)
				}
				else
				{
					// in case "_" not divider: CHESTBIG_NW_NORMAL_LOCKED -> CHESTBIG
					BaseGroupName = AnimationGroupData->GroupName;
				}
			}

			UE_LOG(AssetManager, Log, TEXT("BaseGroupName: %s -> %s"), *AnimationGroupData->GroupName, *BaseGroupName);

			if (!AllAnimationGroupDataMapMap.Contains(BaseGroupName))
				AllAnimationGroupDataMapMap.Add(BaseGroupName, {});

			AllAnimationGroupDataMapMap[BaseGroupName].Add(AnimationGroupData->RelativePath, AnimationGroupData);
			
			// // add to global data
			// if (AssetCategory == EAssetCategory::Shared)
			// {
			// 	if (!SharedMorphDataMap.Contains(GameOrModKey))
			// 		SharedMorphDataMap.Add(GameOrModKey, {});
			// 		
			// 	SharedMorphDataMap[GameOrModKey].Add(*MorphMeshData);
			// }

			// AllMorphMeshDataMap.Add(MorphMeshData->RelativePath, MorphMeshData);
			// 						
			// UE_LOG(AssetManager, Log, TEXT("MorphMeshData->Name: %s"), *MorphMeshData->Name);
		}
	}
}

void UGHAssetManager::ScanSoundFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory,
	const FString& GameOrModKey)
{
	if (GothicHubFolderFullPath.IsEmpty() || FolderRelativePath.IsEmpty() || GameOrModKey.IsEmpty())
		return;
	
	IFileManager& FileManager = IFileManager::Get();

	auto FolderFullPath = FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath);
	
	UE_LOG(AssetManager, Warning, TEXT("Find Sounds folder: %s"), *FolderRelativePath);
	UE_LOG(AssetManager, Warning, TEXT("FolderFullPath: %s"), *FolderFullPath);

	// recusive
	TArray<FString> SubFolderArray;
	FileManager.FindFiles(SubFolderArray, *FPaths::Combine(FolderFullPath, TEXT("*")), false, true);
	for (auto SubFolder : SubFolderArray)
	{
		// SubFolder = folder name

		auto SubFolderRelativePath = FPaths::Combine(FolderRelativePath, SubFolder);
		
		// UE_LOG(AssetManager, Warning, TEXT("Find mesh folder 1111: %s"), *SubFolder);
		// UE_LOG(AssetManager, Warning, TEXT("SubFolderRelativePath: %s"), *SubFolderRelativePath);
		
		ScanSoundFolder(SubFolderRelativePath, AssetCategory, GameOrModKey);
	}

	// scan files in "this" dir
	TArray<FString> FilePathArray;
	FileManager.FindFiles(FilePathArray, *FPaths::Combine(FolderFullPath, TEXT("*")),
		true, false);
	for (auto& FilePath : FilePathArray)
	{
		// FilePath  = file name and file extension
		// FolderRelativePath = file relative path without file name and file extension
		
		auto FileName = FPaths::GetBaseFilename(FilePath);
		auto FileExtension = FPaths::GetExtension(FilePath);

		// UE_LOG(AssetManager, Log, TEXT("FilePath: %s"), *FilePath);

		if (FileExtension == "json")
		{
			auto FileFullPath = FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath, FilePath);
			auto FileRelativeFullPath = FPaths::Combine(FolderRelativePath, FilePath);
			
			UE_LOG(AssetManager, Warning, TEXT("find SoundData file: %s"), *FileRelativeFullPath);
				
			FString JSONFileData;
			if (!FFileHelper::LoadFileToString(JSONFileData, *FileFullPath))
			{
				UE_LOG(AssetManager, Warning, TEXT("Can't read SoundData file: %s"), *FileRelativeFullPath);
				continue;
			}
				
			// UE_LOG(AssetManager, Log, TEXT("GetBaseFilename: %s"), *FPaths::GetBaseFilename(MeshFolderFile));
			
			auto AudioDataArray = GHJSON::ParseAudioData(JSONFileData);
			if (AudioDataArray.Num() <= 0)
			{
				UE_LOG(AssetManager, Warning, TEXT("Can't parse json data in sound data file: %s"),
					*FileRelativeFullPath);
				continue;
			}

			for (auto AudioData : AudioDataArray)
			{
				AudioData->RelativePath = FPaths::Combine(FolderRelativePath, AudioData->Name);
				AudioData->FolderRelativePath = FolderRelativePath;
				AllAudioDataMap.Add(AudioData->RelativePath, AudioData);
			}
		}
	}
}

// need recursive to subfolder
void UGHAssetManager::ScanAnimationFolder(const FString& GothicHubFolderPath, const FString& AnimationsRelativePath,
	bool bIsShared, const FString& Game)
{
	UE_LOG(AssetManager, Log, TEXT("ScanAnimationFolder: %s, %s"), *GothicHubFolderPath, *AnimationsRelativePath);

	IFileManager& FileManager = IFileManager::Get();

	// scan anim groups
	TArray<FString> AnimationsFolderArray;
	FileManager.FindFiles(AnimationsFolderArray,
		*FPaths::Combine(GothicHubFolderPath, AnimationsRelativePath, TEXT("*")), false, true);
	for (auto AnimationsFolder : AnimationsFolderArray)
	{
		auto AnimationsFolderRelativePath = FPaths::Combine(AnimationsRelativePath, AnimationsFolder);

		TArray<FString> AnimationsFolderFileArray;
		FileManager.FindFiles(AnimationsFolderFileArray,
			*FPaths::Combine(GothicHubFolderPath, AnimationsFolderRelativePath, TEXT("*")),
			true, false);
		for (auto AnimationsFolderFile : AnimationsFolderFileArray)
		{
			auto RelativePathToFile = FPaths::Combine(AnimationsFolderRelativePath, AnimationsFolderFile);
			auto FullPathToFile = FPaths::Combine(GothicHubFolderPath, RelativePathToFile);
			if (FPaths::GetExtension(AnimationsFolderFile) == "json")
			{
				UE_LOG(AssetManager, Log, TEXT("find AnimationGroupData file: %s"), *RelativePathToFile);
				
				FString JSONFileData;
				if (!FFileHelper::LoadFileToString(JSONFileData, *FullPathToFile))
				{
					UE_LOG(AssetManager, Warning, TEXT("Can't read AnimationGroupData file: %s"), *RelativePathToFile);
					continue;
				}
				
				UE_LOG(AssetManager, Log, TEXT("GetBaseFilename: %s"), *FPaths::GetBaseFilename(AnimationsFolderFile));

				auto AnimationGroupData = GHJSON::ParseAnimationGroupData(JSONFileData);
				if (!AnimationGroupData || AnimationGroupData->GroupName.IsEmpty())
				{
					UE_LOG(AssetManager, Warning, TEXT("Can't parse json data in AnimationGroupData file: %s"),
						*RelativePathToFile);
					continue;
				}

				AnimationGroupData->RelativePath = AnimationsFolderRelativePath;
				UE_LOG(AssetManager, Log, TEXT("AnimationGroupData->RelativePath: %s"), *AnimationGroupData->RelativePath);

				// MorphGroupData->RelativePath = FPaths::Combine(AnimationsFolderRelativePath,
				// 	FPaths::GetBaseFilename(AnimationsFolderFile));

				// auto MeshFullPathFromMorphMeshData = GetFilePathV2(MeshRelativePathFromMorphMeshData);
				// if (MeshFullPathFromMorphMeshData.IsEmpty())
				// {
				// 	UE_LOG(AssetManager, Warning, TEXT("Can't find file: %s"), *MeshRelativePathFromMorphMeshData);
				// 	continue;
				// }
				// UE_LOG(AssetManager, Log, TEXT("MeshFullPathFromMorphMeshData: %s"), *MeshFullPathFromMorphMeshData);
		
				// auto MorphedSkeletalMesh = UGHGLTFRuntime::LoadMorphedSkeletalMesh(MeshFullPathFromMorphMeshData);
				// if (!MorphedSkeletalMesh)
				// {
				// 	UE_LOG(AssetManager, Warning, TEXT("Can't load morphed skeletal mesh: %s"),
				// 		*MeshRelativePathFromMorphMeshData);
				// 	continue;
				// }
				//
				// MorphGroupData->SkeletalMesh = MorphedSkeletalMesh;
		
				// add to global data
				if (bIsShared)
				{
					// if (!SharedAnimationGroupDataMap.Contains(Game))
					// 	SharedAnimationGroupDataMap.Add(Game, {});

					// SharedAnimationGroupDataMap[Game] = *AnimationGroupData;
					// SharedAnimationGroupDataMap.Add(Game, *AnimationGroupData);
				}
									
				UE_LOG(AssetManager, Log, TEXT("MorphGroupData->Name: %s"), *AnimationGroupData->GroupName);
			}
		}
	}
}

void UGHAssetManager::ScanGameOrModFolder(const FString& FolderRelativePath, EAssetCategory AssetCategory,
	const FString& GameOrModName)
{
	IFileManager& FileManager = IFileManager::Get();

	TArray<FString> FolderArray;
	FileManager.FindFiles(FolderArray, *FPaths::Combine(GothicHubFolderFullPath, FolderRelativePath, TEXT("*")),
		false, true);

	for (auto Folder : FolderArray)
	{
		if (Folder.Compare("Textures", ESearchCase::IgnoreCase) == 0)
		{
			ScanTextureFolder(FPaths::Combine(FolderRelativePath, Folder), AssetCategory, GameOrModName);
		}
	}
	
	for (auto Folder : FolderArray)
	{
		if (Folder.Compare("Meshes", ESearchCase::IgnoreCase) == 0)
		{
			ScanMeshFolder(FPaths::Combine(FolderRelativePath, Folder), AssetCategory, GameOrModName);
		}
	}

	// all after mesh
	
	for (auto Folder : FolderArray)
	{
		if (Folder.Compare("Animations", ESearchCase::IgnoreCase) == 0)
		{
			ScanAnimationFolderV2(FPaths::Combine(FolderRelativePath, Folder), AssetCategory, GameOrModName);
			// ScanAnimationFolder(GothicHubPath, SharedGameFolderRelativePath,
			// 	true, SharedFolder);
		}
	}

	for (auto Folder : FolderArray)
	{
		if (Folder.Compare("Sounds", ESearchCase::IgnoreCase) == 0)
		{
			ScanSoundFolder(FPaths::Combine(FolderRelativePath, Folder), AssetCategory, GameOrModName);
		}
	}
}

void UGHAssetManager::ScanAssetFolder()
{
	UE_LOG(AssetManager, Warning, TEXT("ScanAssetFolder"));
	// TArray<FString> AllFiles;
	// IFileManager::Get().FindFilesRecursive(AllFiles, TEXT("C:/GothicHub/"),
	// 	TEXT("*"), true, false);
	// for (auto File : AllFiles)
	// {
	// 	UE_LOG(AssetManager, Warning, TEXT("find file: %s"), *File);
	// }

	if (GothicHubFolderFullPath.IsEmpty())
		return;

	IFileManager& FileManager = IFileManager::Get();
	
	TArray<FString> GothicHubFolderArray;
	//FString RelativePath = "";
	FileManager.FindFiles(GothicHubFolderArray, *FPaths::Combine(GothicHubFolderFullPath, TEXT("*")),
		false, true);
	for (auto GothicHubFolder : GothicHubFolderArray)
	{
		UE_LOG(AssetManager, Log, TEXT("find folder: %s"), *GothicHubFolder);
		if (GothicHubFolder.Compare("Shared", ESearchCase::IgnoreCase) == 0)
		{
			TArray<FString> SharedFolderArray;
			FileManager.FindFiles(SharedFolderArray,
				*FPaths::Combine(GothicHubFolderFullPath, GothicHubFolder, TEXT("*")),
				false, true);

			// order
			for (FString SharedGame : {"Night of the Raven", "Gothic II", "Gothic", "temp"})
			{
				for (auto SharedFolder : SharedFolderArray)
				{
					if (SharedFolder.Compare(SharedGame, ESearchCase::IgnoreCase) == 0)
					{
						ScanGameOrModFolder(FPaths::Combine(GothicHubFolder, SharedFolder),
							EAssetCategory::Shared, SharedFolder);
					}
				}
			}
		}
		
		// 	TArray<FString> SharedFolderArray;
		// 	// FString RealtivePath = FPaths::Combine(GothicHubPath, TEXT("Shared"));
		// 	UE_LOG(AssetManager, Log, TEXT("GothicHubFolderRelativePath: %s"), *GothicHubFolderRelativePath);
		// 	FileManager.FindFiles(SharedFolderArray,
		// 		*FPaths::Combine(GothicHubPath, GothicHubFolderRelativePath, TEXT("*")),
		// 		false, true);
		// 	for (auto SharedFolder : SharedFolderArray)
		// 	{
		// 		auto SharedFolderRelativePath = FPaths::Combine(GothicHubFolderRelativePath, SharedFolder);
		// 		TArray<FString> SharedGameFolderArray;
		// 		// FString SharedGamePath = FPaths::Combine(SharedPath, SharedFolder);
		// 		UE_LOG(AssetManager, Log, TEXT("SharedFolderRelativePath: %s"), *SharedFolderRelativePath);
		// 		FileManager.FindFiles(SharedGameFolderArray,
		// 			*FPaths::Combine(GothicHubPath, SharedFolderRelativePath, TEXT("*")),
		// 			false, true);
		// 		for (auto SharedGameFolder : SharedGameFolderArray)
		// 		{
		// 			auto SharedGameFolderRelativePath =
		// 				FPaths::Combine(SharedFolderRelativePath, SharedGameFolder);
		// 			if (SharedGameFolder == "Meshes")
		// 			{
		// 				ScanMeshFolder(SharedGameFolderRelativePath, EAssetCategory::Shared, SharedFolder);
		// 			}
		// 			else if (SharedGameFolder == "Animations")
		// 			{
		// 				ScanAnimationFolder(GothicHubPath, SharedGameFolderRelativePath,
		// 					true, SharedFolder);
		// 			}
		// 		}
		// 	}
		//}
	}
}

bool UGHAssetManager::SetupSkeletonSockets(USkeletalMesh* SkeletalMesh, const TMap<FString, FSocketData>& SocketDataMap)
{
	if (!SkeletalMesh)
		return false;
	
	auto Skeleton = SkeletalMesh->GetSkeleton();
	if (!Skeleton)
		return false;

	// UE_LOG(AssetManager, Warning, TEXT("shared skeleton pointer: %p"), Skeleton);
	
	Skeleton->Sockets.Empty();

	TArray<FString> BoneNameArray;
	for (int32 i = 0; i < SkeletalMesh->GetRefSkeleton().GetNum(); i++)
		BoneNameArray.Add(SkeletalMesh->GetRefSkeleton().GetBoneName(i).ToString());

	// for (auto BoneName : BoneNameArray)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "BoneName: " + BoneName);
	// 	UE_LOG(AssetManager, Warning, TEXT("%s"), *BoneName);
	// }
	
	for (auto SocketDataTuple : SocketDataMap)
	{
		auto SocketData = SocketDataTuple.Value;
	
		if (!BoneNameArray.Contains(SocketData.Bone))
			continue;

		auto Socket = NewObject<USkeletalMeshSocket>(Skeleton);
		if (!Socket)
			continue;

		if (SocketData.Name.IsEmpty())
			continue;

		if (SocketData.Bone.IsEmpty())
			continue;
		
		Socket->SocketName = FName(SocketData.Name);
		Socket->BoneName = FName(SocketData.Bone);
		Socket->RelativeLocation = SocketData.Position;
		Socket->RelativeRotation = SocketData.Rotation;

		Skeleton->Sockets.Add(Socket);
	}

	if (Skeleton->Sockets.Num() != SocketDataMap.Num())
	{
		UE_LOG(AssetManager, Warning, TEXT("Some socket not added"));
		return false;
	}

	return true;
}

bool UGHAssetManager::SetupSkeletonSlots(USkeleton* Skeleton)
{
	if (!Skeleton)
		return false;

	// @TODO: add all slots, mb all
	TArray<FString> SlotArray = {"1", "2", "4", "9", "11", "20"};
	
	for (auto Slot : SlotArray)
	{
		FString GroupName = "G" + Slot;
		Skeleton->AddSlotGroupName(FName(GroupName));
		if (Skeleton->FindAnimSlotGroup(FName(GroupName)))
		{
			FString SlotName = "S" + Slot;
			Skeleton->SetSlotGroupName(FName(SlotName), FName(GroupName));

			if (!Skeleton->ContainsSlotName(FName(SlotName)))
			{
				UE_LOG(AssetManager, Warning, TEXT("Slot name not added: %s"), *SlotName);
				return false;
			}
		}
		else
		{
			UE_LOG(AssetManager, Warning, TEXT("Slot group not added: %s"), *GroupName);
			return false;
		}
	}

	return true;
}

bool UGHAssetManager::SetupAnimationGroupData(FAnimationGroupData* AnimationGroupData)
{
	auto SkeletalMeshData = GetSkeletalMeshV2(AnimationGroupData->SkeletonFileName);
	if (!SkeletalMeshData || !SkeletalMeshData->IsValid())
	{
		UE_LOG(AssetManager, Warning, TEXT("AnimationGroupData not setup"));
		return false;
	}

	TArray<USkeletalMesh*> SkeletalMeshArray;
	SkeletalMeshData->SkeletalMeshMap.GenerateValueArray(SkeletalMeshArray);
	
	auto SkeletalMesh = SkeletalMeshArray[0];
	if (!SkeletalMesh)
	{
		UE_LOG(AssetManager, Warning, TEXT("AnimationGroupData not setup"));
		return false;
	}

	
	//get skeletal mesh

	// LoadSkeletalMesh(const FString& PathToFile, USkeleton* Skeleton)
	
	//GetSke

	// const FString& PathToFile, USkeleton* Skeleton
	// auto SkeletalMesh = LoadSkeletalMesh(PathToFile, USkeleton* Skeleton);
	// if (!SkeletalMesh)
	// 	return false;
	
	// "skeleton": "HUM_BODY_NAKED0",
	
	auto Skeleton = SkeletalMesh->GetSkeleton();
	if (!Skeleton)
	{
		UE_LOG(AssetManager, Warning, TEXT("AnimationGroupData not setup"));
		return false;
	}
	
	// TMap<int32, FString> BoneNameMap = {};
	// for (int32 BoneIndex = 0; BoneIndex < SkeletalMesh->GetRefSkeleton().GetNum(); BoneIndex++)
	// 	BoneNameMap.Add(BoneIndex, SkeletalMesh->GetRefSkeleton().GetBoneName(BoneIndex).ToString());
			
	auto bIsSocketsAdded = SetupSkeletonSockets(SkeletalMesh, AnimationGroupData->SocketDataMap);
	auto bIsSlotsAdded = SetupSkeletonSlots(Skeleton);
	
	if (!bIsSocketsAdded || !bIsSlotsAdded)
	{
		UE_LOG(AssetManager, Warning, TEXT("AnimationGroupData not setup"));
		return false;
	}
	
	AnimationGroupData->SkeletalMesh = SkeletalMesh;
	AnimationGroupData->SharedSkeleton = Skeleton;
	
	return true;
}

bool UGHAssetManager::SetupAnimationData(UAnimationData* AnimationData, USkeletalMesh* SkeletalMesh)
{
	// setup reverse
	if (AnimationData->bIsReverse)
	{
		AnimationData->AnimSequence->RateScale = -1.0f;

		// fix all events
		auto AnimSequenceLength = AnimationData->AnimSequence->GetPlayLength();
		for (int32 i = 0; i < AnimationData->Events.Num(); i++)
		{
			auto EventTimeReverse = AnimSequenceLength - AnimationData->Events[i].Time;
			if (EventTimeReverse > 0)
			{
				AnimationData->Events[i].Time = EventTimeReverse;
			}
		}
	}

	// get transformed bones
	if (SkeletalMesh && AnimationData->Layer != "1")
	{
		for (int32 i = 0; i < SkeletalMesh->GetRefSkeleton().GetNum(); i++)
		{
			FTransform PrevBoneTransform;
			FTransform CurrBoneTransform;
			FSkeletonPoseBoneIndex BoneIndex = FSkeletonPoseBoneIndex(i);
			auto AnimSequencePlayLength = AnimationData->AnimSequence->GetPlayLength();
			for (double Time = 0.0; Time < AnimSequencePlayLength; Time += 0.1) // 10 fps
			{
				AnimationData->AnimSequence->GetBoneTransform(CurrBoneTransform, BoneIndex, Time, true);

				if (Time == 0.0)
				{
					PrevBoneTransform = CurrBoneTransform;
					continue;
				}
						
				if (!PrevBoneTransform.Equals(CurrBoneTransform, 0.001))
				{
					PrevBoneTransform = CurrBoneTransform;
					AnimationData->BlendBones.Add(SkeletalMesh->GetRefSkeleton().GetBoneName(i).ToString());
					break;
				}
				else
				{
					PrevBoneTransform = CurrBoneTransform;
				}
			}
		}
	}
	
	return true;
}

FMorphMeshData UGHAssetManager::GetMorphedSkeletalMesh(const FString& RelativePath)
{
	for (auto SharedGameTuple : SharedMorphDataMap)
	{
		auto SharedGameArray = SharedGameTuple.Value;
		for (auto MorphData : SharedGameArray)
		{
			if (RelativePath.Compare(MorphData.Name, ESearchCase::IgnoreCase) == 0 ||
				RelativePath.Compare(MorphData.RelativePath, ESearchCase::IgnoreCase) == 0)
			{
				if (MorphData.SkeletalMesh)
					return MorphData;

				auto MeshFullPathFromMorphMeshData = GetFileFullPathV2(MorphData.RelativePath);
				if (MeshFullPathFromMorphMeshData.IsEmpty())
				{
					UE_LOG(AssetManager, Warning, TEXT("Can't find file: %s"), *MorphData.RelativePath);
					return FMorphMeshData();
				}
				UE_LOG(AssetManager, Log, TEXT("MeshFullPathFromMorphMeshData: %s"), *MeshFullPathFromMorphMeshData);
				UE_LOG(AssetManager, Log, TEXT("MorphData.RelativePath: %s"), *MorphData.RelativePath);
				
				auto MorphedSkeletalMesh = LoadMorphedSkeletalMesh(MeshFullPathFromMorphMeshData);
				if (!MorphedSkeletalMesh)
				{
					UE_LOG(AssetManager, Warning, TEXT("Can't load morphed skeletal mesh: %s"),
						*MorphData.RelativePath);
					return FMorphMeshData();
				}
				
				MorphData.SkeletalMesh = MorphedSkeletalMesh;
				
				return MorphData;
			}
		}
	}

	return FMorphMeshData();
}

FMorphMeshData* UGHAssetManager::GetMorphedMeshV2(const FString& RelativePath)
{
	// RelativePath can be:
	// ITRW_BOW_H_01
	// Shared\Night of the Raven\Meshes\ITRW_BOW_H_01\ITRW_BOW_H_01
	// Games\Archolos\Meshes\ITRW_BOW_H_01\ITRW_BOW_H_01
	// Games\Archolos\Mods\ModName\ITRW_BOW_H_01\ITRW_BOW_H_01
	
	auto MorphMeshData = AllMorphMeshDataMap.FindRef(RelativePath);
	if (MorphMeshData)
	{
		// found by RelativePath
		
		if (MorphMeshData->IsValid())
		{
			return MorphMeshData;
		}
		else
		{
			// found, but mesh not load (first time get)

			if (MorphMeshData->Name.IsEmpty() || MorphMeshData->RelativePath.IsEmpty())
				return nullptr;
			
			// dont work, return empty string
			// auto MeshFilePath = FPaths::Combine(FPaths::GetPath(MorphMeshDataTuple.Value->RelativePath),
			// 	MorphMeshDataTuple.Value->Path);
			
			auto MeshFullPath = GetFileFullPathV2(MorphMeshData->RelativePath);
			if (MeshFullPath.IsEmpty())
				return nullptr;

			auto MorphedSkeletalMesh = LoadMorphedSkeletalMesh(MeshFullPath);
			if (!MorphedSkeletalMesh)
				return nullptr;

			MorphMeshData->SkeletalMesh = MorphedSkeletalMesh;

			// auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
			// if (bIsOnlyName)
			// 	AllMorphMeshDataMap.Add(MorphMeshData->Name, MorphMeshData);

			return MorphMeshData;
		}
	}
	else
	{
		// или имя, или ошибка в пути
		// если ошибкап в пути то мы просто не находим ничего
		// так что все действия идут только когда имя
		for (auto& MorphMeshDataTuple : AllMorphMeshDataMap)
		{
			if (RelativePath.Compare(MorphMeshDataTuple.Value->Name, ESearchCase::IgnoreCase) == 0 ||
				RelativePath.Compare(MorphMeshDataTuple.Value->RelativePath, ESearchCase::IgnoreCase) == 0)
			{
				if (MorphMeshDataTuple.Value->IsValid())
				{
					AllMorphMeshDataMap.Add(MorphMeshDataTuple.Value->Name, MorphMeshDataTuple.Value);
					
					return MorphMeshDataTuple.Value;
				}
				else
				{
					// found, but mesh not load (first time get)

					if (MorphMeshDataTuple.Value->Name.IsEmpty() || MorphMeshDataTuple.Value->RelativePath.IsEmpty())
						continue;

					// dont work, return empty string
					// auto MeshFilePath = FPaths::Combine(FPaths::GetPath(MorphMeshDataTuple.Value->RelativePath),
					// 	MorphMeshDataTuple.Value->Path);
				
					auto MeshFullPath = GetFileFullPathV2(MorphMeshDataTuple.Value->RelativePath);
					if (MeshFullPath.IsEmpty())
						continue;

					auto MorphedSkeletalMesh = LoadMorphedSkeletalMesh(MeshFullPath);
					if (!MorphedSkeletalMesh)
						continue;

					MorphMeshDataTuple.Value->SkeletalMesh = MorphedSkeletalMesh;

					// auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
					// if (bIsOnlyName)
					AllMorphMeshDataMap.Add(MorphMeshDataTuple.Value->Name, MorphMeshDataTuple.Value);

					return MorphMeshDataTuple.Value;
				}
			}
		}
	}
		
	
	// auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
	// if (!bIsOnlyName)
	// {
	// 	if (!MorphMeshData)
	// 		return nullptr;
	// }

	// // AllMorphMeshDataMap have order!!!
	// for (auto& MorphMeshDataTuple : AllMorphMeshDataMap)
	// {
	// 	if (RelativePath.Compare(MorphMeshDataTuple.Value->Name, ESearchCase::IgnoreCase) == 0)
	// 	{
	// 		auto MeshFullPath = GetFilePathV2(MorphMeshDataTuple.Value->RelativePath);
	// 		auto MeshFullPath = FPaths::Combine(GothicHubFolderFullPath, );
	// 		auto StaticMesh = LoadMorphedSkeletalMesh(FPaths::Combine(GothicHubFolderFullPath, RelativePath));
	// 		if (StaticMesh)
	// 		{
	//
	// 		}
	// 	}
	// }


	// if (MorphMeshData->SkeletalMesh)
	// 	return MorphMeshData;

	
	// MorphMeshData found, but skeleton mesh not loaded



	// Нужно строго соблюдать порядок, Shared (в правильном порядке) -> Game -> Mods (в правильном порядке)
	// При цикле начинаем проверять с модов (в обратном порядке)
	// for (auto& UniversalMeshInfoMapTuple : UniversalMeshInfoMap)
	// {
	// 	if (RelativePath.Compare(UniversalMeshInfoMapTuple.Value.FileName, ESearchCase::IgnoreCase) == 0 ||
	// 		RelativePath.Compare(UniversalMeshInfoMapTuple.Value.RelativePath, ESearchCase::IgnoreCase) == 0)
	// 	{
	// 		// with FileExtension
	// 		auto FileFullPath = FPaths::Combine(GothicHubFolderFullPath,
	// 			UniversalMeshInfoMapTuple.Value.RelativePath);
	// 		FileFullPath = FileFullPath + TEXT(".") + UniversalMeshInfoMapTuple.Value.FileExtension;
	//
	// 		auto StaticMesh = LoadStaticMesh(FileFullPath);
	// 		if (StaticMesh)
	// 		{
	// 			auto StructCppOps = FStaticMeshData::StaticStruct()->GetCppStructOps();
	// 			auto StaticMeshData = static_cast<FStaticMeshData*>(FMemory::Malloc(
	// 				StructCppOps->GetSize(), StructCppOps->GetAlignment()));
	// 			StructCppOps->Construct(StaticMeshData);
	//
	// 			StaticMeshData->Name = UniversalMeshInfoMapTuple.Value.FileName;
	// 			StaticMeshData->RelativePath = UniversalMeshInfoMapTuple.Value.RelativePath;
	// 			StaticMeshData->StaticMesh = StaticMesh;
	//
	// 			// ???
	//
	// 			if (bIsOnlyName)
	// 				AllStaticMeshDataMap.Add(StaticMeshData->Name, StaticMeshData);
	// 			
	// 			AllStaticMeshDataMap.Add(StaticMeshData->RelativePath, StaticMeshData);
	//
	// 			return StaticMeshData;
	// 		}
	// 	}
	// }
	
	return nullptr;
}

UAnimationData* UGHAssetManager::GetAnimationFromAnimationGroupData(FAnimationGroupData* AnimationGroupData,
	const FString& RelativePath, const bool bIsRelativePath)
{
	if (!AnimationGroupData)
		return nullptr;
	
	// name only
	if (!bIsRelativePath)
	{
		return AnimationGroupData->AnimationDataMap.FindRef(RelativePath);
	}
	// path and name
	else
	{
		for (auto& AnimationDataTuple : AnimationGroupData->AnimationDataMap)
		{
			auto& AnimationData = AnimationDataTuple.Value;
			if (RelativePath.Compare(FPaths::Combine(AnimationGroupData->RelativePath, AnimationData->Name),
				ESearchCase::IgnoreCase) == 0)
			{
				return AnimationData;
			}
		}
	}

	return nullptr;
}

UAnimationData* UGHAssetManager::GetAnimationV2(const FString& GroupName, const FString& RelativePath)
{
	// PlayAnim("dance")
	// temporary only name, no time for this
	
	if (GroupName.IsEmpty() || RelativePath.IsEmpty())
		return nullptr;
	
	auto BaseAllAnimationGroupDataMap = AllAnimationGroupDataMapMap.Find(GroupName);
	if (!BaseAllAnimationGroupDataMap || BaseAllAnimationGroupDataMap->Num() <= 0)
		return nullptr;

	UE_LOG(AssetManager, Log, TEXT("GetAnimationV2: %s, %s"), *GroupName, *RelativePath);
	
	// base skeleton name


	// for (auto& AllAnimationGroupDataMapTuple : AllAnimationGroupDataMapMap)
	// {
	// 	auto& AllAnimationGroupDataMap = AllAnimationGroupDataMapTuple.Value;
	// 	FString BaseGroupName;
	//
	// 	AllAnimationGroupDataMap
	// }
	// FString InPath = "Shared/Night of the Raven/Animations/Humans/dance"; 
	//
	// FString Path = FPaths::GetPath(InPath);
	// FString Filename = FPaths::GetBaseFilename(InPath);
	//
	// UE_LOG(AssetManager, Warning, TEXT("111 Path: %s"), *Path); // Shared/Night of the Raven/Animations/Humans
	// UE_LOG(AssetManager, Warning, TEXT("111 Filename: %s"), *Filename); // dance

	auto AnimationGroupDataMain = BaseAllAnimationGroupDataMap->FindRef(GroupName);
	if (!AnimationGroupDataMain)
	{
		for (auto& AnimationGroupDataTuple : *BaseAllAnimationGroupDataMap)
		{
			if (AnimationGroupDataTuple.Value->GroupName.Equals(GroupName, ESearchCase::IgnoreCase))
			{
				AnimationGroupDataMain = AnimationGroupDataTuple.Value;
				break;
			}
		}
	}
	
	if (!AnimationGroupDataMain)
		return nullptr;

	if (!AnimationGroupDataMain->SkeletalMesh || !AnimationGroupDataMain->SharedSkeleton)
	{
		auto bIsSetupAnimationGroupData = SetupAnimationGroupData(AnimationGroupDataMain);
		if (!bIsSetupAnimationGroupData)
			return nullptr;
	}

	for (auto& AnimationGroupDataTuple : *BaseAllAnimationGroupDataMap)
	{
		if (AnimationGroupDataTuple.Key.Contains("HUMANS_1HST2") && RelativePath == "S_1HRUN")
		{
			UE_LOG(AssetManager, Log, TEXT("bingo"));
		}
		// Shared/temp/Animations/Humans_1Hst2/HUMANS_1HST2, S_1HRUN
		// key = Shared/temp/Animations/Humans/HUMANS
		UE_LOG(AssetManager, Log, TEXT("GetAnimationV2 Key, RelPath: %s, %s"), *AnimationGroupDataTuple.Key, *RelativePath);
		auto AnimationGroupData = AnimationGroupDataTuple.Value;

		// UE_LOG(AssetManager, Warning, TEXT("111 AnimationGroupData Key: %s"), *AnimationGroupDataTuple.Key); // dance
		
		auto AnimationData = AnimationGroupData->AnimationDataMap.FindRef(RelativePath);
		if (!AnimationData)
			continue;

		// SEQ!!!
		if (AnimationData->AnimSequence)
			return AnimationData;

		auto AnimationFullPath = GetFileFullPathV2(FPaths::Combine(
			FPaths::GetPath(AnimationGroupData->RelativePath), AnimationData->Name));
		if (AnimationFullPath.IsEmpty())
		{
			// UE_LOG(AssetManager, Warning, TEXT("Can't find file: %s"),
			// 	*(FPaths::Combine(AnimationGroupData->RelativePath, AnimationData->Name)));
			continue;
		}
		
		// UE_LOG(AssetManager, Log, TEXT("AnimationFullPath: %s"), *AnimationFullPath);

		if (!AnimationGroupData->SkeletalMesh || !AnimationGroupData->SharedSkeleton)
		{
			if (AnimationGroupDataMain->SkeletalMesh && AnimationGroupDataMain->SharedSkeleton)
			{
				AnimationGroupData->SkeletalMesh = AnimationGroupDataMain->SkeletalMesh;
				AnimationGroupData->SharedSkeleton = AnimationGroupDataMain->SharedSkeleton;
			}
			else
			{
				return nullptr;
			}
			// auto bIsSetupAnimationGroupData = SetupAnimationGroupData(AnimationGroupData);
			// if (!bIsSetupAnimationGroupData)
			// 	continue;
		}
		
		// if (!AnimationGroupData->SkeletalMesh || !AnimationGroupData->SharedSkeleton)
		// 	continue;
		
		auto AnimSequence = LoadAnimSequence(AnimationGroupData->SkeletalMesh, AnimationFullPath);
		if (!AnimSequence)
			continue;
		
		AnimationData->AnimSequence = AnimSequence;
		
		SetupAnimationData(AnimationData, AnimationGroupData->SkeletalMesh);

		UE_LOG(AssetManager, Log, TEXT("GetAnimationV2 return ok: %s, %s"), *AnimationGroupDataTuple.Key, *RelativePath);

		return AnimationData;
	}
	// auto AnimationGroupData = AllAnimationGroupDataMap->FindRef(RelativePath);
	// if (AnimationGroupData)
	// {
	// 	
	// }
	// {
	// 	// found by RelativePath
	// 	// перебирать не нужно, и так нашли
	//
	// 	// если RelativePath путь, то нужно отделить анимацию...
	// 	auto AnimationData = AnimationGroupData->AnimationDataMap.Find(RelativePath);
	// 	if (AnimationData->IsValid())
	// 	{
	// 		return AnimationData;
	// 	}
	// 	else
	// 	{
	// 		// found, but mesh not load (first time get)
	//
	// 		if (AnimationData->Name.IsEmpty() || AnimationData->RelativePath.IsEmpty())
	// 			return nullptr;
	// 		
	// 		// dont work, return empty string
	// 		// auto MeshFilePath = FPaths::Combine(FPaths::GetPath(MorphMeshDataTuple.Value->RelativePath),
	// 		// 	MorphMeshDataTuple.Value->Path);
	// 		
	// 		auto MeshFullPath = GetFileFullPathV2(AnimationData->RelativePath);
	// 		if (MeshFullPath.IsEmpty())
	// 			return nullptr;
	//
	// 		auto MorphedSkeletalMesh = LoadMorphedSkeletalMesh(MeshFullPath);
	// 		if (!MorphedSkeletalMesh)
	// 			return nullptr;
	//
	// 		AnimationData->SkeletalMesh = MorphedSkeletalMesh;
	//
	// 		// auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
	// 		// if (bIsOnlyName)
	// 		// 	AllMorphMeshDataMap.Add(MorphMeshData->Name, MorphMeshData);
	//
	// 		return AnimationData;
	// 	}
	// }
	// else
	// {
	// 	// или имя, или ошибка в пути
	// 	// если ошибкап в пути то мы просто не находим ничего
	// 	// так что все действия идут только когда имя
	// 	for (auto& MorphMeshDataTuple : AllMorphMeshDataMap)
	// 	{
	// 		if (RelativePath.Compare(MorphMeshDataTuple.Value->Name, ESearchCase::IgnoreCase) == 0 ||
	// 			RelativePath.Compare(MorphMeshDataTuple.Value->RelativePath, ESearchCase::IgnoreCase) == 0)
	// 		{
	// 			if (MorphMeshDataTuple.Value->IsValid())
	// 			{
	// 				AllMorphMeshDataMap.Add(MorphMeshDataTuple.Value->Name, MorphMeshDataTuple.Value);
	// 				
	// 				return MorphMeshDataTuple.Value;
	// 			}
	// 			else
	// 			{
	// 				// found, but mesh not load (first time get)
	//
	// 				if (MorphMeshDataTuple.Value->Name.IsEmpty() || MorphMeshDataTuple.Value->RelativePath.IsEmpty())
	// 					continue;
	//
	// 				// dont work, return empty string
	// 				// auto MeshFilePath = FPaths::Combine(FPaths::GetPath(MorphMeshDataTuple.Value->RelativePath),
	// 				// 	MorphMeshDataTuple.Value->Path);
	// 			
	// 				auto MeshFullPath = GetFileFullPathV2(MorphMeshDataTuple.Value->RelativePath);
	// 				if (MeshFullPath.IsEmpty())
	// 					continue;
	//
	// 				auto MorphedSkeletalMesh = LoadMorphedSkeletalMesh(MeshFullPath);
	// 				if (!MorphedSkeletalMesh)
	// 					continue;
	//
	// 				MorphMeshDataTuple.Value->SkeletalMesh = MorphedSkeletalMesh;
	//
	// 				// auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
	// 				// if (bIsOnlyName)
	// 				AllMorphMeshDataMap.Add(MorphMeshDataTuple.Value->Name, MorphMeshDataTuple.Value);
	//
	// 				return MorphMeshDataTuple.Value;
	// 			}
	// 		}
	// 	}
	// }

	return nullptr;
}

USkeletalMesh* UGHAssetManager::GetVisual(const FString& GroupName)
{
	if (GroupName.IsEmpty())
		return nullptr;
	
	auto AllAnimationGroupDataMap = AllAnimationGroupDataMapMap.Find(GroupName);
	if (!AllAnimationGroupDataMap || AllAnimationGroupDataMap->Num() <= 0)
		return nullptr;

	for (auto& AnimationGroupDataTuple : *AllAnimationGroupDataMap)
	{
		if (AnimationGroupDataTuple.Value->SkeletalMesh && AnimationGroupDataTuple.Value->SharedSkeleton)
		{
			return AnimationGroupDataTuple.Value->SkeletalMesh;
		}
		else
		{
			auto bIsSetupAnimationGroupData = SetupAnimationGroupData(AnimationGroupDataTuple.Value);
			if (!bIsSetupAnimationGroupData)
				return nullptr;

			if (AnimationGroupDataTuple.Value->SkeletalMesh && AnimationGroupDataTuple.Value->SharedSkeleton)
			{
				return AnimationGroupDataTuple.Value->SkeletalMesh;
			}
		}
	}

	return nullptr;
}

UAnimationData* UGHAssetManager::GetAnimation(const FString& GroupName, const FString& RelativePath)
{
	// PlayAnim("dance")
	// PlayAnim("Shared\Night of the Raven\Animations\Humans/dance")
	// PlayAnim("Games\Archolos\Animations\Humans/dance")
	// PlayAnim("Games\Archolos\Mods\ModName\Animations\Humans/dance")
	
	if (GroupName.IsEmpty() || RelativePath.IsEmpty())
		return nullptr;

	// use "one ring to rule them all"
	//
	// auto AnimationData = AnimationDataMap.FindRef("[" + GroupName + "]" + RelativePath);
	// if (AnimationData)
	// 	return AnimationData;
	//
	// // dont find, check all
	//
	// auto bIsRelativePath = !FPaths::GetPath(RelativePath).IsEmpty();
	//
	// FAnimationGroupData* AnimationGroupData = nullptr;
	// for (auto ModAnimationGroupDataMapTuple : ModAnimationGroupDataMapMap)
	// {
	// 	auto ModAnimationGroupDataMap = ModAnimationGroupDataMapTuple.Value;
	// 	AnimationGroupData = ModAnimationGroupDataMap.Find(GroupName);
	// 	if (AnimationGroupData)
	// 	{
	// 		AnimationData = GetAnimationFromAnimationGroupData(AnimationGroupData,
	// 			RelativePath, bIsRelativePath);
	// 		if (AnimationData)
	// 			break;
	// 	}
	// }
	//
	// if (!AnimationData)
	// {
	// 	// fix me @TODO: 
	// 	
	// 	// for (auto GameAnimationGroupDataMapTuple : GameAnimationGroupDataMap)
	// 	// {
	// 	// 	auto GameAnimationGroupDataMap = GameAnimationGroupDataMapTuple.Value;
	// 	// 	AnimationGroupData = GameAnimationGroupDataMap.Find(GroupName);
	// 	// 	if (AnimationGroupData)
	// 	// 	{
	// 	// 		AnimationData = GetAnimationFromAnimationGroupData(AnimationGroupData,
	// 	// 			RelativePath, bIsRelativePath);
	// 	// 		if (AnimationData)
	// 	// 			break;
	// 	// 	}
	// 	// }
	// }
	//
	// if (!AnimationData)
	// {
	// 	for (auto SharedAnimationGroupDataMapTuple : SharedAnimationGroupDataMapMap)
	// 	{
	// 		auto SharedAnimationGroupDataMap = SharedAnimationGroupDataMapTuple.Value;
	// 		AnimationGroupData = SharedAnimationGroupDataMap.Find(GroupName);
	// 		if (AnimationGroupData)
	// 		{
	// 			AnimationData = GetAnimationFromAnimationGroupData(AnimationGroupData,
	// 				RelativePath, bIsRelativePath);
	// 			if (AnimationData)
	// 				break;
	// 		}
	// 	}
	// }
	//
	// if (!AnimationData)
	// 	return nullptr;
	//
	// // SEQ!!!
	// if (AnimationData->AnimSequence)
	// 	return AnimationData;
	//
	// auto AnimationFullPath = GetFileFullPathV2(FPaths::Combine(AnimationGroupData->RelativePath,
	// 	AnimationData->Name));
	// if (AnimationFullPath.IsEmpty())
	// {
	// 	UE_LOG(AssetManager, Warning, TEXT("Can't find file: %s"),
	// 		*(FPaths::Combine(AnimationGroupData->RelativePath, AnimationData->Name)));
	// 	return nullptr;
	// }
	//
	// UE_LOG(AssetManager, Log, TEXT("AnimationFullPath: %s"), *AnimationFullPath);
	//
	// auto bIsSetupAnimationGroupData = SetupAnimationGroupData(AnimationGroupData);
	// if (!bIsSetupAnimationGroupData)
	// 	return nullptr;
	//
	// if (!AnimationGroupData->SkeletalMesh || !AnimationGroupData->SharedSkeleton)
	// 	return nullptr;
	//
	// auto AnimSequence = LoadAnimSequence(AnimationGroupData->SkeletalMesh, AnimationFullPath);
	// if (!AnimSequence)
	// 	return nullptr;
	//
	// AnimationData->AnimSequence = AnimSequence;
	// SetupAnimationData(AnimationData, AnimationGroupData->SkeletalMesh);
	//
	// AnimationDataMap.Add("[" + GroupName + "]" + RelativePath, AnimationData);
	//
	// return AnimationData;

	return nullptr;
}

FStaticMeshData* UGHAssetManager::GetStaticMesh(const FString& RelativePath)
{
	// RelativePath can be:
	// ItMw_1H_Axe_01
	// Shared\Night of the Raven\Meshes\ItMw_1H_Axe_01
	// Games\Archolos\Mods\ModName\ItMw_1H_Axe_01
	
	if (GothicHubFolderFullPath.IsEmpty() || RelativePath.IsEmpty())
		return nullptr;
	
	for (auto& SharedGameTuple : SharedStaticMeshDataMap)
	{
		UE_LOG(AssetManager, Warning, TEXT("GetStaticMesh, check SharedGameTuple: %s"), *SharedGameTuple.Key);
		for (auto& StaticMeshData : SharedGameTuple.Value)
		{
			UE_LOG(AssetManager, Warning, TEXT("SharedGameArray, StaticMeshData.Name: %s"), *StaticMeshData.Name);

			UE_LOG(AssetManager, Warning, TEXT("start compare: %s==%s==%s"), *RelativePath, *StaticMeshData.Name, *StaticMeshData.RelativePath);
			
			if (RelativePath.Compare(StaticMeshData.Name, ESearchCase::IgnoreCase) == 0 ||
				RelativePath.Compare(StaticMeshData.RelativePath, ESearchCase::IgnoreCase) == 0)
			{
				UE_LOG(AssetManager, Warning, TEXT("compare ok: %s==%s==%s"), *RelativePath, *StaticMeshData.Name, *StaticMeshData.RelativePath);
				
				if (StaticMeshData.StaticMesh)
					return &StaticMeshData;

				auto MeshFullPathFromStaticMeshData = GetFileFullPathV2(StaticMeshData.RelativePath);
				if (MeshFullPathFromStaticMeshData.IsEmpty())
				{
					UE_LOG(AssetManager, Warning, TEXT("Can't find file: %s"), *StaticMeshData.RelativePath);
					return nullptr;
				}
				UE_LOG(AssetManager, Log, TEXT("MeshFullPathFromMorphMeshData: %s"), *MeshFullPathFromStaticMeshData);
				UE_LOG(AssetManager, Log, TEXT("StaticMeshData.RelativePath: %s"), *StaticMeshData.RelativePath);
				
				auto StaticMesh = LoadStaticMesh(MeshFullPathFromStaticMeshData);
				if (!StaticMesh)
				{
					UE_LOG(AssetManager, Warning, TEXT("Can't load morphed skeletal mesh: %s"),
						*StaticMeshData.RelativePath);
					return nullptr;
				}
				
				StaticMeshData.StaticMesh = StaticMesh;
				
				return &StaticMeshData;
			}
		}
	}

	// file not found, need add
	for (auto& UniversalMeshDataTuple : UniversalMeshInfoMap)
	{
		if (RelativePath.Compare(UniversalMeshDataTuple.Value.FileName, ESearchCase::IgnoreCase) == 0 ||
			RelativePath.Compare(UniversalMeshDataTuple.Value.RelativePath, ESearchCase::IgnoreCase) == 0)
		{
			// found, need add to map

			// with FileExtension
			auto FileFullPath = FPaths::Combine(GothicHubFolderFullPath,
				UniversalMeshDataTuple.Value.RelativePath);
			FileFullPath = FileFullPath + TEXT(".") + UniversalMeshDataTuple.Value.FileExtension;

			auto StaticOrSkeletalMesh = LoadStaticOrSkeletalMesh(FileFullPath);
			if (!StaticOrSkeletalMesh)
				return nullptr;

			auto StaticMesh = Cast<UStaticMesh>(StaticOrSkeletalMesh);
			if (StaticMesh)
			{
				auto AssetCategory = UniversalMeshDataTuple.Value.AssetCategory;
				if (AssetCategory == EAssetCategory::Shared)
				{
					FStaticMeshData StaticMeshData;
					StaticMeshData.Name = UniversalMeshDataTuple.Value.FileName;
					StaticMeshData.RelativePath = UniversalMeshDataTuple.Value.RelativePath;
					StaticMeshData.StaticMesh = StaticMesh;

					// GameOrModKey take in UniversalMeshData
					// SharedStaticMeshDataMap.Add(GameOrModKey, StaticMeshData);
				}
			}

			auto SkeletalMesh = Cast<USkeletalMesh>(StaticOrSkeletalMesh);
		}
	}

	return nullptr;
}

FStaticMeshData* UGHAssetManager::GetStaticMeshV2(const FString& RelativePath)
{
	// RelativePath can be:
	// ItMw_1H_Axe_01
	// Shared\Night of the Raven\Meshes\ItMw_1H_Axe_01
	// Games\Archolos\Meshes\ItMw_1H_Axe_01
	// Games\Archolos\Mods\ModName\ItMw_1H_Axe_01
	
	auto ReturnStaticMeshData = AllStaticMeshDataMap.FindRef(RelativePath);
	if (ReturnStaticMeshData)
		return ReturnStaticMeshData;

	auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
	
	// file not found, need add

	// Нужно строго соблюдать порядок, Shared (в правильном порядке) -> Game -> Mods (в правильном порядке)
	// При цикле начинаем проверять с модов (в обратном порядке)
	for (auto& UniversalMeshInfoMapTuple : UniversalMeshInfoMap)
	{
		if (RelativePath.Compare(UniversalMeshInfoMapTuple.Value.FileName, ESearchCase::IgnoreCase) == 0 ||
			RelativePath.Compare(UniversalMeshInfoMapTuple.Value.RelativePath, ESearchCase::IgnoreCase) == 0)
		{
			// can some optimize, if found, check one more time AllStaticMeshDataMap.FindRef
			
			// with FileExtension GetFilePathV2(const FString& RelativePath, TArray<FString> FileFormatArray)
			// auto FileFullPath = FPaths::Combine(GothicHubFolderFullPath,
			// 	UniversalMeshInfoMapTuple.Value.RelativePath);
			// FileFullPath = FileFullPath + TEXT(".") + UniversalMeshInfoMapTuple.Value.FileExtension;

			auto FileFullPath = GetFileFullPathV2(UniversalMeshInfoMapTuple.Value.RelativePath);
			if (FileFullPath.IsEmpty())
				continue;

			auto StaticMesh = LoadStaticMesh(FileFullPath);
			if (StaticMesh)
			{
				auto StructCppOps = FStaticMeshData::StaticStruct()->GetCppStructOps();
				auto StaticMeshData = static_cast<FStaticMeshData*>(FMemory::Malloc(
					StructCppOps->GetSize(), StructCppOps->GetAlignment()));
				StructCppOps->Construct(StaticMeshData);

				StaticMeshData->Name = UniversalMeshInfoMapTuple.Value.FileName;
				StaticMeshData->RelativePath = UniversalMeshInfoMapTuple.Value.RelativePath;
				StaticMeshData->StaticMesh = StaticMesh;

				// ???

				if (bIsOnlyName)
					AllStaticMeshDataMap.Add(StaticMeshData->Name, StaticMeshData);
				
				AllStaticMeshDataMap.Add(StaticMeshData->RelativePath, StaticMeshData);

				return StaticMeshData;
			}
		}
	}
	
	return nullptr;
}

FSkeletalMeshData* UGHAssetManager::GetSkeletalMeshV2(const FString& RelativePath)
{
	// RelativePath can be:
	// ItMw_1H_Axe_01
	// Shared\Night of the Raven\Meshes\ItMw_1H_Axe_01
	// Games\Archolos\Meshes\ItMw_1H_Axe_01
	// Games\Archolos\Mods\ModName\ItMw_1H_Axe_01
	
	auto ReturnSkeletalMeshData = AllSkeletalMeshDataMap.FindRef(RelativePath);
	if (ReturnSkeletalMeshData)
		return ReturnSkeletalMeshData;
	
	// file not found, need add

	// Нужно строго соблюдать порядок, Shared (в правильном порядке) -> Game -> Mods (в правильном порядке)
	// При цикле начинаем проверять с модов (в обратном порядке)
	for (auto& UniversalMeshInfoTuple : UniversalMeshInfoMap)
	{
		if (RelativePath.Compare(UniversalMeshInfoTuple.Value.FileName, ESearchCase::IgnoreCase) == 0 ||
			RelativePath.Compare(UniversalMeshInfoTuple.Value.RelativePath, ESearchCase::IgnoreCase) == 0)
		{
			// can some optimize, if found, check one more time AllStaticMeshDataMap.FindRef
			
			// with FileExtension GetFilePathV2(const FString& RelativePath, TArray<FString> FileFormatArray)
			// auto FileFullPath = FPaths::Combine(GothicHubFolderFullPath,
			// 	UniversalMeshInfoMapTuple.Value.RelativePath);
			// FileFullPath = FileFullPath + TEXT(".") + UniversalMeshInfoMapTuple.Value.FileExtension;

			auto FileFullPath = GetFileFullPathV2(UniversalMeshInfoTuple.Value.RelativePath);
			if (FileFullPath.IsEmpty())
				continue;

			auto SkeletalMeshMap = LoadSkeletalMesh(FileFullPath, nullptr);
			if (SkeletalMeshMap.Num() <= 0)
				continue;

			auto StructCppOps = FSkeletalMeshData::StaticStruct()->GetCppStructOps();
			auto SkeletalMeshData = static_cast<FSkeletalMeshData*>(FMemory::Malloc(
				StructCppOps->GetSize(), StructCppOps->GetAlignment()));
			StructCppOps->Construct(SkeletalMeshData);

			SkeletalMeshData->Name = UniversalMeshInfoTuple.Value.FileName;
			SkeletalMeshData->RelativePath = UniversalMeshInfoTuple.Value.RelativePath;
			SkeletalMeshData->SkeletalMeshMap = SkeletalMeshMap;

			// ???
			
			auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
			if (bIsOnlyName)
				AllSkeletalMeshDataMap.Add(SkeletalMeshData->Name, SkeletalMeshData);
			
			AllSkeletalMeshDataMap.Add(SkeletalMeshData->RelativePath, SkeletalMeshData);

			return SkeletalMeshData;
		}
	}
	
	return nullptr;
}

FTextureData* UGHAssetManager::GetTextureData(const FString& RelativePath, int32 Variant)
{
	// // for static
	// if (!GameInstance)
	// {
	// 	GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	// 	if (!GameInstance)
	// 		return nullptr;
	// }

	FString RelativePathNew = RelativePath;
	if (Variant >= 0)
	{
		int32 IndexStart = -1;
		int32 IndexEnd = -1;
		IndexStart = RelativePath.Find(TEXT("_V"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (IndexStart > 0)
		{
			IndexEnd = RelativePath.Find(TEXT("_"),
				ESearchCase::IgnoreCase, ESearchDir::FromStart, IndexStart + 1);
			if (IndexEnd < 0)
			{
				IndexEnd = RelativePath.Find(TEXT("."),
					ESearchCase::IgnoreCase, ESearchDir::FromStart, IndexStart + 1);
				if (IndexEnd < 0)
				{
					IndexEnd = RelativePath.Len();
				}
			}
		}
		if (IndexStart > 0 && IndexEnd > 0 && IndexEnd > IndexStart + 2)
		{
			RelativePathNew = RelativePath.Mid(0, IndexStart + 2);
			RelativePathNew += FString::Printf(TEXT("%i"), Variant);
			RelativePathNew += RelativePath.Mid(IndexEnd, RelativePath.Len());

			UE_LOG(AssetManager, Warning, TEXT("Texture replaced variant: %s -> %s"), *RelativePath, *RelativePathNew);
		}
	}
	
	auto ReturnTextureData = AllTextureDataMap.Find(RelativePathNew);
	if (ReturnTextureData)
	{
		// found by RelativePath
		
		if (ReturnTextureData->IsValid())
		{
			return ReturnTextureData;
		}

		for (auto& TextureDataMapTuple : AllTextureDataMap)
		{
			auto& TextureData = TextureDataMapTuple.Value;
			if (RelativePathNew.Compare(TextureData.Name, ESearchCase::IgnoreCase) == 0 ||
				RelativePathNew.Compare(TextureData.RelativePath, ESearchCase::IgnoreCase) == 0)
			{
				if (TextureData.IsValid())
				{
					AllTextureDataMap.Add(TextureData.Name, TextureData);
					return &TextureData;
				}

				auto TextureFileFullPath = GetFileFullPathV2(TextureData.RelativePath);
				if (TextureFileFullPath.IsEmpty())
					continue;
				
				// found, but audio files not load (first time get)
				auto Texture2D = FImageUtils::ImportFileAsTexture2D(TextureFileFullPath);
				if (!Texture2D)
					continue;

				TextureData.Texture = Texture2D;
				AllTextureDataMap.Add(TextureData.Name, TextureData);
				
				return &TextureData;
			}
		}
	}

	return nullptr;
}

FAudioData* UGHAssetManager::GetAudioData(const FString& RelativePath, bool bPlayAfterLoad, FVector* Location)
{
	auto ReturnAudioData = AllAudioDataMap.FindRef(RelativePath);
	if (ReturnAudioData)
	{
		// found by RelativePath
		
		if (ReturnAudioData->IsValid())
		{
			return ReturnAudioData;
		}
		else
		{
			// found, but audio files not load (first time get)

			if (ReturnAudioData->Name.IsEmpty() || ReturnAudioData->RelativePath.IsEmpty())
				return nullptr;
			
			// dont work, return empty string
			// auto MeshFilePath = FPaths::Combine(FPaths::GetPath(MorphMeshDataTuple.Value->RelativePath),
			// 	MorphMeshDataTuple.Value->Path);

			auto PathToJSONFolder = FPaths::GetPath(ReturnAudioData->RelativePath);
			for (auto& AudioFile : ReturnAudioData->FileArray)
			{
				auto AudioFileRelativeFullPath = FPaths::Combine(PathToJSONFolder, AudioFile.Name);
				auto AudioFileFullPath = GetFileFullPathV2(ReturnAudioData->RelativePath);
				if (AudioFileFullPath.IsEmpty())
					continue;

				// FAudioFile* AudioFilePtr = &AudioFile;
				LoadAudioFileAsync(AudioFileFullPath, ReturnAudioData, &AudioFile, bPlayAfterLoad, Location);
				bPlayAfterLoad = false;

				// auto  = LoadMorphedSkeletalMesh(AudioFileFullPath);
				// if (!MorphedSkeletalMesh)
				// 	return nullptr;

				// AudioData->SkeletalMesh = MorphedSkeletalMesh;

				// auto bIsOnlyName = FPaths::GetPath(RelativePath).IsEmpty();
				// if (bIsOnlyName)
				// 	AllMorphMeshDataMap.Add(MorphMeshData->Name, MorphMeshData);
			}
			
			return ReturnAudioData;
		}
	}
	else
	{
		// или имя, или ошибка в пути
		// если ошибкап в пути то мы просто не находим ничего
		// так что все действия идут только когда имя
		for (auto& AudioDataMapTuple : AllAudioDataMap)
		{
			auto AudioData = AudioDataMapTuple.Value;
			if (RelativePath.Compare(AudioData->Name, ESearchCase::IgnoreCase) == 0 ||
				RelativePath.Compare(AudioData->RelativePath, ESearchCase::IgnoreCase) == 0)
			{
				if (AudioData->IsValid())
				{
					AllAudioDataMap.Add(AudioData->Name, AudioData);
					
					return AudioData;
				}
				else
				{
					// found, but audio files not load (first time get)

					if (AudioData->Name.IsEmpty() || AudioData->RelativePath.IsEmpty())
						return nullptr;

					// dont work, return empty string
					// auto MeshFilePath = FPaths::Combine(FPaths::GetPath(MorphMeshDataTuple.Value->RelativePath),
					// 	MorphMeshDataTuple.Value->Path);

					for (auto& AudioFile : AudioData->FileArray)
					{
						// dont work, return invalid? string
						// auto PathToJSONFolder = FPaths::GetPath(AudioData->RelativePath);
						// combine dont work too
						// auto AudioFileRelativeFullPath = FPaths::Combine(AudioData->FolderRelativePath,
						// 	AudioFile.Name);
						// auto AudioFileRelativeFullPath =
						// 	AudioData->FolderRelativePath + TEXT("/") + AudioFile.Name;

						// auto AudioFileRelativeFullPath = AudioData->FolderRelativePath + TEXT("/") + AudioFile.Name;

						auto PathToJSONFolder = FPaths::GetPath(AudioData->RelativePath);
						auto AudioFileRelativeFullPath = FPaths::Combine(PathToJSONFolder,
							AudioFile.Name);
						
						// auto TestString = FString::Printf(TEXT("%s/%s"),
						// 	TEXT("Shared/temp/Sounds"), *AudioFile.Name);
						//
						// auto TestString2 = FPaths::Combine(AudioData->FolderRelativePath, AudioFile.Name);

						// UE_LOG(AssetManager, Warning, TEXT("TestString: %s"), *PathToJSONFolder);
						// UE_LOG(AssetManager, Warning, TEXT("TestString2: %s"), *AudioFileRelativeFullPath);

						// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "TestString2 " + AudioFileRelativeFullPath);
						
						auto AudioFileFullPath = GetFileFullPathV2(AudioFileRelativeFullPath);
						if (AudioFileFullPath.IsEmpty())
							continue;

						// UE_LOG(AssetManager, Warning, TEXT("TestString3: %s"), *AudioFileFullPath);
						// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "TestString3 " + AudioFileFullPath);
						
						// FAudioFile* AudioFilePtr = &AudioFile;
						LoadAudioFileAsync(AudioFileFullPath, AudioData, &AudioFile, bPlayAfterLoad, Location);
						bPlayAfterLoad = false;
					}
				}
			}
		}
	}

	return nullptr;
}

// UObject* UGHAssetManager::GetStaticMeshOrSkeletonMesh(const FString& RelativePath)
// {
// 	auto StaticMesh = GetStaticMesh(RelativePath);
// 	if (StaticMesh)
// 		return StaticMesh;
// }

// FixWaterMaterial()
// {
// 	material->SetVectorParameterValue(FName(TEXT("BaseColor")), FLinearColor(0.9f, 0.1f, 0.1f));
// 	UMaterialInstanceDynamic::SetTextureParameterValue
// }

// void UGHAssetManager::LoadMorphMesh(const FString& PathFileFolder, const FString& FileName)
// {
// 	// load json
// }

FString UGHAssetManager::GetFilePath(const FString& RelativePath, TArray<FString> FileFormatArray)
{
	if (PathToFolder.IsEmpty())
		return "";

	if (RelativePath.IsEmpty())
		return "";

	// file format already in relative path
	if (FileFormatArray.Num() <= 0)
	{
		auto PathToFile = PathToFolder + RelativePath;
		if (FPaths::FileExists(PathToFile))
		{
			return PathToFile;
		}
		else
		{
			return "";
		}
	}
	else
	{
		for (auto FileFormat : FileFormatArray)
		{
			auto PathToFile = PathToFolder + RelativePath + "." + FileFormat;
			if (FPaths::FileExists(PathToFile))
			{
				return PathToFile;
			}
		}
	}

	return "";
}

FString UGHAssetManager::GetFileFullPathV2(const FString& RelativePath, TArray<FString> FileFormatArray)
{
	if (RelativePath.IsEmpty())
		return "";

	TArray<FString> PathToFolderArray = {"C:/archolos/", "C:/GothicHub/", "C:/gothic_lua_scripts/"};

	if (FPaths::GetExtension(RelativePath).IsEmpty() && FileFormatArray.Num() <= 0)
	{
		FileFormatArray = {"glb", "gltf", "wav", "png"};
	}

	// @TODO: replace + to combine
	// file format already in relative path
	if (FileFormatArray.Num() <= 0)
	{
		for (auto PathToFolderE : PathToFolderArray)
		{
			auto PathToFile = PathToFolderE + RelativePath;
			if (FPaths::FileExists(PathToFile))
			{
				return PathToFile;
			}
		}
	}
	else
	{
		for (auto PathToFolderE : PathToFolderArray)
		{
			for (auto FileFormat : FileFormatArray)
			{
				auto PathToFile = PathToFolderE + RelativePath + "." + FileFormat;
				// UE_LOG(AssetManager, Warning, TEXT("check: %s"), *PathToFile);
				if (FPaths::FileExists(PathToFile))
				{
					return PathToFile;
				}
			}
		}
	}

	return "";
}

// ####
// GLTFRuntime
// ####

TMap<FString, UStaticMesh*> UGHAssetManager::LoadWorld(const FString& WorldName)
{
	TMap<FString, UStaticMesh*> ReturnMap = {};
	// FPaths::Combine(BaseDir, TEXT("Platforms"));
	
	auto PathToFile = GetFilePath("worlds/gltf/" + WorldName, {"glb", "gltf"});
	if (PathToFile.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find file: %s"), *WorldName);
		return ReturnMap;
	}

	UStaticMesh* WorldStaticMesh = nullptr;
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false,
		LoaderConfig);
	if (RuntimeAsset)
	{
		TArray<FString> WorldNodes = {"WATER", "ALPHA"};
		FglTFRuntimeStaticMeshConfig StaticMeshConfig;
		for (auto Node : WorldNodes)
		{
			StaticMeshConfig.bBuildSimpleCollision = Node.Contains("COLLISION") ? true : false;
			auto StaticMesh = RuntimeAsset->LoadStaticMeshByNodeName(Node, StaticMeshConfig);
			// auto StaticMesh = RuntimeAsset->LoadStaticMesh(0, StaticMeshConfig);
			if (StaticMesh)
			{
				ReturnMap.Add(Node, StaticMesh);
			}
		}
		
		// StaticMeshConfig.bBuildSimpleCollision = false;
		// auto StaticMesh = RuntimeAsset->LoadStaticMeshByNodeName("WATER", StaticMeshConfig);
		// // auto StaticMesh = RuntimeAsset->LoadStaticMesh(0, StaticMeshConfig);
		// if (StaticMesh)
		// {
		// 	ReturnMap.Add("WATER", StaticMesh);
			// if (!WaterMaterial)
			// 	return StaticMesh;
			//
			// for (int32 i = 0; true; i++)
			// {
			// 	auto MaterialInterface = StaticMesh->GetMaterial(i);
			// 	if (!MaterialInterface)
			// 		break;
			//
			// 	StaticMesh->SetMaterial(i, MaterialInterface);
			// 	continue;
			//
			// 	auto MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(MaterialInterface);
			// 	if (!MaterialInstanceDynamic)
			// 		break;
			//
			// 	//static FName ExponentName("Opacity");
			//
			// 	// ComponentMaterial = UMaterialInstanceDynamic::Create(
			// 	// 	BodyPosition->BodyComponent->GetMaterial(0) , GetWorld());
			// 	// BodyPosition->BodyComponent->SetMaterial(0, ComponentMaterial);
			//
			//
			// 	TArray<UTexture*> UsedTextures;
			// 	MaterialInstanceDynamic->GetUsedTextures(UsedTextures, EMaterialQualityLevel::Epic,
			// 		true, ERHIFeatureLevel::SM6, true);
			//
			// 	UTexture* BaseColorTexture = nullptr;
			// 	for (auto Texture : UsedTextures)
			// 	{
			// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Texture NAME: " +
			// 		Texture->GetName());
			//
			// 		if (Texture->GetName().Contains("2D"))
			// 			BaseColorTexture = Texture;
			// 	}
			// 	
			// 	auto NewMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(WaterMaterial, this);
			// 	// MaterialInstanceDynamic->SetTextureParameterValue(FName("Base Texture"), BaseColorTexture);
			// 	// MaterialInstanceDynamic->SetScalarParameterValue(FName("Opacity"), 0.5f);
			//
			// 	StaticMesh->SetMaterial(i, NewMaterialInstanceDynamic);
			// 	
			// 	//
			// 	// MaterialInstanceDynamic->BlendMode = EBlendMode::BLEND_Translucent;
			// 	// MaterialInstanceDynamic->SetScalarParameterValue(FName("Opacity"), 0.5f);
			// 	// auto OVal = MaterialInstanceDynamic->SetScalarParameterValue ( FName ParameterName, float Value )
			// 	//SetScalarParameterValue ( FName("Opacity"), 0.8f)
			// 	// UMaterialInstanceDynamic::SetTextureParameterValue
			//
			// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "MaterialInterface NAME: " +
			// 		MaterialInterface->GetName());
			// }
			// StaticMesh->GetMaterial()
			// TArray<UMaterialInterface*>& OutMaterials;
			// StaticMesh->GetUsedMaterials(TArray<UMateriOutMaterials, bool bGetDebugMaterials = false) const override;
			// return ReturnMap;
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Can't load water from asset: %s"), *WorldName);
		// 	return ReturnMap;
		// }
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't load asset from file: %s"), *WorldName);
		return ReturnMap;
	}

	return ReturnMap;
}

TMap<FString, USkeletalMesh*> UGHAssetManager::LoadSkeletalMesh(const FString& PathToFile, USkeleton* Skeleton)
{
	TMap<FString, USkeletalMesh*> ReturnMap = {};
	
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false,
		LoaderConfig);
	if (!RuntimeAsset)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load asset from: %s"), *PathToFile);
		return ReturnMap;
	}

	// By default glTFRuntime will create a new Skeleton for each SkeletalMesh,
	// you can force a specific Skeleton (useful for sharing animation) by setting the specific options:
	FglTFRuntimeSkeletalMeshConfig RuntimeSkeletalMeshConfig;

	// // not help for only one mesh bug
	// RuntimeSkeletalMeshConfig.CacheMode = EglTFRuntimeCacheMode::None;
	
	if (Skeleton)
	{
		RuntimeSkeletalMeshConfig.Skeleton = Skeleton;
		// RuntimeSkeletalMeshConfig.bOverwriteRefSkeleton = true;
	}
	
	auto NodeArray = RuntimeAsset->GetNodes();
	for (auto& Node : NodeArray)
	{
		if (Node.MeshIndex < 0)
			continue;
		
		auto SkeletalMesh = RuntimeAsset->LoadSkeletalMesh(Node.MeshIndex, 0, RuntimeSkeletalMeshConfig);
		if (!SkeletalMesh)
		{
			UE_LOG(AssetManager, Warning, TEXT("Can't load skeletal mesh from asset: %s"), *PathToFile);
			continue;
		}

		ReturnMap.Add(Node.Name, SkeletalMesh);
	}

	// UE_LOG(AssetManager, Warning, TEXT("MaterialLen: %i"), RuntimeAsset->MaterialsMap.Num());
	// for (auto& MatetialInterfaceTuple : RuntimeAsset->MaterialsMap)
	// {
	// 	UE_LOG(AssetManager, Warning, TEXT("MaterialName: %s"), *MatetialInterfaceTuple.Value->GetName());
	// }

	return ReturnMap;
}

USkeletalMesh* UGHAssetManager::LoadMorphedSkeletalMesh(const FString& PathToFile)
{
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false,
		LoaderConfig);
	if (!RuntimeAsset)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load asset from: %s"), *PathToFile);
		return nullptr;
	}

	FglTFRuntimeSkeletalMeshConfig RuntimeSkeletalMeshConfig;
	// fix twitch/tremble
	RuntimeSkeletalMeshConfig.bIgnoreEmptyMorphTargets = false;
	auto SkeletalMesh = RuntimeAsset->LoadSkeletalMesh(0, INDEX_NONE, RuntimeSkeletalMeshConfig);
	if (!SkeletalMesh)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load skeletal mesh from asset: %s"), *PathToFile);
		return nullptr;
	}

	return SkeletalMesh;
}

UAnimSequence* UGHAssetManager::LoadAnimSequence(USkeletalMesh* SkeletalMesh, const FString& PathToFile)
{
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false,
		LoaderConfig);
	if (!RuntimeAsset)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load asset from: %s"), *PathToFile);
		return nullptr;
	}
	
	FglTFRuntimeSkeletalAnimationConfig RSAConfig;
	RSAConfig.bRootMotion = true;
	auto AnimSequence = RuntimeAsset->LoadSkeletalAnimation(SkeletalMesh, 0, RSAConfig);
	if (!AnimSequence)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load animation from asset: %s"), *PathToFile);
		return nullptr;
	}

	return AnimSequence;
}

UStaticMesh* UGHAssetManager::LoadStaticMesh(const FString& PathToFile)
{
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false,
		LoaderConfig);
	if (!RuntimeAsset)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load asset from: %s"), *PathToFile);
		return nullptr;
	}
	
	FglTFRuntimeStaticMeshConfig StaticMeshConfig;
	StaticMeshConfig.bBuildSimpleCollision = true;
	auto StaticMesh = RuntimeAsset->LoadStaticMesh(0, StaticMeshConfig);
	if (!StaticMesh)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load static mesh from asset: %s"), *PathToFile);
		return nullptr;
	}

	return StaticMesh;
}

UObject* UGHAssetManager::LoadStaticOrSkeletalMesh(const FString& PathToFile)
{
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false,
		LoaderConfig);
	if (!RuntimeAsset)
	{
		UE_LOG(AssetManager, Warning, TEXT("Can't load asset from: %s"), *PathToFile);
		return nullptr;
	}
	
	FglTFRuntimeStaticMeshConfig StaticMeshConfig;
	StaticMeshConfig.bBuildSimpleCollision = true;
	auto StaticMesh = RuntimeAsset->LoadStaticMesh(0, StaticMeshConfig);
	if (StaticMesh)
		return StaticMesh;
	
	int32 SkinIndex = 0;
	FglTFRuntimeSkeletalMeshConfig RuntimeSkeletalMeshConfig;
	// collisison?
	if (RuntimeAsset->MeshHasMorphTargets(0))
	{
		// fix twitch/tremble
		RuntimeSkeletalMeshConfig.bIgnoreEmptyMorphTargets = false;
		SkinIndex = INDEX_NONE;
	}
	
	auto SkeletalMesh = RuntimeAsset->LoadSkeletalMesh(0, SkinIndex, RuntimeSkeletalMeshConfig);
	if (SkeletalMesh)
		return SkeletalMesh;
	
	UE_LOG(AssetManager, Warning, TEXT("Can't load any mesh: %s"), *PathToFile);
	
	return nullptr;
}

// ####
// ----
// ####

// ####
// RuntimeAudioImporter
// ####

void UGHAssetManager::LoadAudioFileAsync(const FString& PathToFile, FAudioData* AudioData, FAudioFile* AudioFile,
	bool bPlayAfterLoad, FVector* Location)
{
	if (PathToFile.IsEmpty() || !AudioData || !AudioFile)
		return;
	
	auto RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	if (RuntimeAudioImporter)
	{
		RuntimeAudioImporter->OnResultNative.AddWeakLambda(this,
			[this, AudioData, AudioFile, bPlayAfterLoad, Location]
			(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave,
				ERuntimeImportStatus Status)
		{
			if (Status == ERuntimeImportStatus::SuccessfulImport)
			{
				auto SoundWaveProcedural = Cast<USoundWaveProcedural>(ImportedSoundWave);
				if (!SoundWaveProcedural)
					return;

				// Duplicete not work???
				// AudioFile->SoundWaveProcedural = DuplicateObject<USoundWaveProcedural>(SoundWaveProcedural, this);
				// if (!AudioFile->SoundWaveProcedural)
				// 	return;

				// FOnDuplicateSoundWave OnDuplicateSoundWave;
				//
				// // OnDuplicateSoundWave.BindUFunction(this, "MyUFunction");
				// OnDuplicateSoundWave.BindDynamic(this, {
				// 	
				// });
				
				// Del.BindLambda([ActorsArray](int32 ActorIndex)
				// {
				// 	//Print the name of the Actor which has an index equal to the one we provided (ActorIndex)
				// 	//Make sure we provided a valid index for our array
				// 	if (ActorsArray.IsValidIndex(ActorIndex))
				// 		GLog->Log("Actor with given index:" + ActorsArray[ActorIndex]->GetName());
				// 	else
				// 		GLog->Log("You've entered an invalid index. That's unfortunate :(");
				// });
				
				// ImportedSoundWave->DuplicateSoundWave += [&] { DoSomeStuff(); };

				
				// ImportedSoundWave->DuplicateSoundWave(false, [] () {});

				ImportedSoundWave->DuplicateSoundWave(false,
					FOnDuplicateSoundWaveNative::CreateWeakLambda(this,
					[this, AudioData, AudioFile, bPlayAfterLoad, Location]
					(bool bSucceeded, UImportedSoundWave* DuplicatedSoundWave)
				{
						if (!bSucceeded)
							return;
						
						AudioFile->ImportedSoundWave = DuplicatedSoundWave;
						AudioData->bIsLoaded = true;

						if (bPlayAfterLoad)
						{
							if (Location)
							{
								// play 3D
							}
							else
							{
								PlayAudio2D(AudioData, AudioFile);
								GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "PlayAudio2D OK ");
							}
						}
				}));


				
				// ImportedSoundWave->DuplicateSoundWave(false, OnDuplicateSoundWave);
				// ImportedSoundWave->DuplicateSoundWave(false, [&] (bool bSucceeded, UImportedSoundWave* DuplicatedSoundWave) {});

				//void UImportedSoundWave::DuplicateSoundWave(bool bUseSharedAudioBuffer, const FOnDuplicateSoundWave& Result)

				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "DuplicateObject OK ");
				
				// for (FAudioFile& AudioFile : AudioData->FileArray)
				// {
				// 	if (AudioFile.Name != FileName)
				// 		continue;
				// 						
				// 	if (AudioFile.SoundWaveProcedural)
				// 		continue;
				// 	
				// 	AudioFile.SoundWaveProcedural = ImportedSoundWave;
				// }
			}
	
			// Destroy object ???
		});
	}
	
	if (RuntimeAudioImporter)
	{
		RuntimeAudioImporter->ImportAudioFromFile(PathToFile, ERuntimeAudioFormat::Auto);
	}
}

void UGHAssetManager::LoadAllAudioInAudioDataMap()
{
	// for (const TTuple<FString, FAudioData>& AudioDataTuple : AudioDataMap)
	// {
	// 	for (const FAudioFile& AudioFile : AudioDataTuple.Value.FileArray)
	// 	{
	// 		if (AudioFile.SoundWaveProcedural)
	// 			continue;
	//
	// 		// if (!AudioFile.Name.Contains("STEP"))
	// 		// 	continue;
	//
	// 		LoadAudioInAudioDataMap(AudioDataTuple.Value.Name, AudioFile.Name);
	// 	}
	// }
}

void UGHAssetManager::LoadAudioInAudioDataMap(const FString& AudioName, const FString& FileName)
{
	auto PathToAudioFile = GetFilePath("sounds/" + FileName, {});
	
	if (PathToAudioFile.IsEmpty())
		return;
	//
	// auto RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	// if (RuntimeAudioImporter)
	// {
	// 	RuntimeAudioImporter->OnResultNative.AddWeakLambda(this, [this, AudioName, FileName]
	// 		(URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave,
	// 			ERuntimeImportStatus Status)
	// 	{
	// 		if (Status == ERuntimeImportStatus::SuccessfulImport)
	// 		{
	// 			auto SoundWaveProcedural = Cast<USoundWaveProcedural>(ImportedSoundWave);
	// 			if (SoundWaveProcedural)
	// 			{
	// 				auto AudioData = AudioDataMap.Find(AudioName);
	// 				if (!AudioData)
	// 					return;
	//
	// 				for (FAudioFile& AudioFile : AudioData->FileArray)
	// 				{
	// 					if (AudioFile.Name != FileName)
	// 						continue;
	// 										
	// 					if (AudioFile.SoundWaveProcedural)
	// 						continue;
	// 					
	// 					AudioFile.SoundWaveProcedural = ImportedSoundWave;
	// 				}
	// 			}
	// 		}
	//
	// 		// Destroy object ???
	// 	});
	// }
	//
	// if (RuntimeAudioImporter)
	// {
	// 	RuntimeAudioImporter->ImportAudioFromFile(PathToAudioFile, ERuntimeAudioFormat::Auto);
	// }
}

void UGHAssetManager::PlayAudio2D(FAudioData* AudioData, FAudioFile* AudioFile)
{
	if (!AudioData)
		return;

	UImportedSoundWave* ImportedSoundWave = nullptr;
	if (AudioFile)
	{
		ImportedSoundWave = Cast<UImportedSoundWave>(AudioFile->ImportedSoundWave);
	}
	else
	{
		auto RandomIndex = FMath::RandRange(0, AudioData->FileArray.Num() - 1);
		ImportedSoundWave = Cast<UImportedSoundWave>(AudioData->FileArray[RandomIndex].ImportedSoundWave);
	}
	
	if (!ImportedSoundWave)
		return;
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return;
	
	auto Audio = UGameplayStatics::CreateSound2D(World, ImportedSoundWave);
	if (Audio)
	{
		Audio->VolumeMultiplier = 0.4f;
		Audio->bAutoDestroy = true;
		Audio->Play();
	}
}

void UGHAssetManager::PlayAudioByName2D(const FString& RelativePath)
{
	auto AudioData = GetAudioData(RelativePath, true, nullptr);
	if (AudioData)
	{
		PlayAudio2D(AudioData, nullptr);
	}
	else
	{
		// play it inside GetAudioData
	}
	
	// auto AudioData = AudioDataMap.Find(AudioName);
	// if (!AudioData)
	// 	return;
	//
	// auto RandomIndex = FMath::RandRange(0, AudioData->FileArray.Num() - 1);
	// if (!AudioData->FileArray[RandomIndex].SoundWaveProcedural)
	// 	return;
	//
	// auto Audio = UGameplayStatics::CreateSound2D(World, AudioData->FileArray[RandomIndex].SoundWaveProcedural);
	// if (Audio)
	// {
	// 	Audio->VolumeMultiplier = 0.4f;
	// 	Audio->bAutoDestroy = true;
	// 	Audio->Play();
	// }
}

// ####
// ----
// ###