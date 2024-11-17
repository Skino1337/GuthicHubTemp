// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GHGLTFRuntime.h"


// DEFINE_LOG_CATEGORY(GLTF);


UGHGLTFRuntime::UGHGLTFRuntime()
{
    LoadInfoArray = {};
}

FString UGHGLTFRuntime::PrepareFilePath(const FString& RelativePath)
{
    auto PathToFolder = "C:/archolos/";
    auto PathToFile = PathToFolder + RelativePath + ".gltf";
    if (!FPaths::FileExists(PathToFile))
    {
        PathToFile = PathToFolder + RelativePath + ".glb";
        if (!FPaths::FileExists(PathToFile))
        {
            return "";
        }
    }

    return PathToFile;
}

USkeletalMesh* UGHGLTFRuntime::LoadSkeletalMesh(const FString& FileName, USkeleton* Skeleton)
{
    auto PathToFile = PrepareFilePath("test/" + FileName);
    if (PathToFile.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't find file: %s"), *FileName);
        return nullptr;
    }
    
    FglTFRuntimeConfig LoaderConfig;
    LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false, LoaderConfig);
    if (!RuntimeAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't load asset from: %s"), *FileName);
        return nullptr;
    }

    // By default glTFRuntime will create a new Skeleton for each SkeletalMesh,
    // you can force a specific Skeleton (useful for sharing animation) by setting the specific options:
    FglTFRuntimeSkeletalMeshConfig RuntimeSkeletalMeshConfig;
    if (Skeleton)
    {
        RuntimeSkeletalMeshConfig.Skeleton = Skeleton;
        // RuntimeSkeletalMeshConfig.bOverwriteRefSkeleton = true;
    }
    auto LoadedSkeletalMesh = RuntimeAsset->LoadSkeletalMesh(0, 0, RuntimeSkeletalMeshConfig);
    if (!LoadedSkeletalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("!LoadedSkeletalMesh"));
        return nullptr;
    }
    
    // SkeletalMeshComponent->SetSkeletalMesh(LoadedSkeletalMesh);

    return LoadedSkeletalMesh;
}

USkeletalMesh* UGHGLTFRuntime::LoadMorphedSkeletalMesh(const FString& PathToFile)
{
    FglTFRuntimeConfig LoaderConfig;
    LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false, LoaderConfig);
    if (!RuntimeAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't load asset from: %s"), *PathToFile);
        return nullptr;
    }

    FglTFRuntimeSkeletalMeshConfig RuntimeSkeletalMeshConfig;
    // fix twitch/tremble
    RuntimeSkeletalMeshConfig.bIgnoreEmptyMorphTargets = false;
    auto SkeletalMesh = RuntimeAsset->LoadSkeletalMesh(0, 0, RuntimeSkeletalMeshConfig);
    if (!SkeletalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("!LoadedSkeletalMesh %s"), *PathToFile);
        return nullptr;
    }

    return SkeletalMesh;
}

UAnimSequence* UGHGLTFRuntime::LoadAnimSequence(USkeletalMesh* SkeletalMesh, const FString& FileName)
{
    auto PathToFile = PrepareFilePath("animations/gltf/" + FileName);
    if (PathToFile.IsEmpty())
    {
        PathToFile = PrepareFilePath("test/" + FileName);
        if (PathToFile.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Can't find file: %s"), *FileName);
            return nullptr;
        }
    }
    
    FglTFRuntimeConfig LoaderConfig;
    LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false, LoaderConfig);
    if (!RuntimeAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't load asset from: %s"), *FileName);
        return nullptr;
    }
	
    FglTFRuntimeSkeletalAnimationConfig RSAConfig;
    RSAConfig.bRootMotion = true;
    auto AnimSequence = RuntimeAsset->LoadSkeletalAnimation(SkeletalMesh, 0, RSAConfig);
    if (!AnimSequence)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't load animation from: %s"), *FileName);
        return nullptr;
    }

    return AnimSequence;
}

UAnimSequence* UGHGLTFRuntime::LoadAnimSequenceForBlendSpace(USkeletalMesh* SkeletalMesh, const FString& FileName)
{
    auto PathToFile = PrepareFilePath("animations/gltf/" + FileName);
    if (PathToFile.IsEmpty())
    {
        PathToFile = PrepareFilePath("test/" + FileName);
        if (PathToFile.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Can't find file: %s"), *FileName);
            return nullptr;
        }
    }
    
    FglTFRuntimeConfig LoaderConfig;
    LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::IdentityXInverted;
    auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFile,false, LoaderConfig);
    if (!RuntimeAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't load asset from: %s"), *FileName);
        return nullptr;
    }
	
    FglTFRuntimeSkeletalAnimationConfig RSAConfig;
    RSAConfig.bRootMotion = false;
    auto AnimSequence = RuntimeAsset->LoadSkeletalAnimation(SkeletalMesh, 0, RSAConfig);
    if (!AnimSequence)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't load animation from: %s"), *FileName);
        return nullptr;
    }

    return AnimSequence;
}

bool UGHGLTFRuntime::LoadModel(UStaticMeshComponent* StaticMeshComponent, FString ModelName, bool Collision)
{
    for (auto LoadInfo : LoadInfoArray)
    {
        if (LoadInfo.Name == ModelName && LoadInfo.Collision == Collision)
        {
            if (LoadInfo.StaticMesh)
            {
                StaticMeshComponent->SetStaticMesh(LoadInfo.StaticMesh);
                return true;
            }
        }
    }

    // auto PathToModel = "C:/" + ModelName + ".gltf";
    auto PathToModel = "C:/archolos/models/gltf/" + ModelName + ".gltf";
    FglTFRuntimeConfig LoaderConfig;
    // LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToModel,false, LoaderConfig);
    if (RuntimeAsset)
    {
        FglTFRuntimeStaticMeshConfig StaticMeshConfig;
        StaticMeshConfig.bBuildSimpleCollision = Collision;
        auto StaticMesh = RuntimeAsset->LoadStaticMesh(0, StaticMeshConfig);
        if (StaticMesh)
        {
            auto LoadInfo = FLoadInfo();
            LoadInfo.Name = ModelName;
            LoadInfo.Collision = Collision;
            LoadInfo.StaticMesh = StaticMesh;
            LoadInfoArray.Add(LoadInfo);
            
            StaticMeshComponent->SetStaticMesh(StaticMesh);
            return true;
        }
    }

    return false;
}

bool UGHGLTFRuntime::LoadModelYF(UStaticMeshComponent* StaticMeshComponent, FString ModelName, bool Collision)
{
    for (auto LoadInfo : LoadInfoArray)
    {
        if (LoadInfo.Name == ModelName && LoadInfo.Collision == Collision)
        {
            if (LoadInfo.StaticMesh)
            {
                StaticMeshComponent->SetStaticMesh(LoadInfo.StaticMesh);
                return true;
            }
        }
    }
    
    // auto PathToModel = "C:/" + ModelName + ".gltf";
    auto PathToModel = "C:/archolos/models/gltf/" + ModelName + ".gltf";
    FglTFRuntimeConfig LoaderConfig;
    LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    auto RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToModel,false, LoaderConfig);
    if (RuntimeAsset)
    {
        FglTFRuntimeStaticMeshConfig StaticMeshConfig;
        StaticMeshConfig.bBuildSimpleCollision = Collision;
        auto StaticMesh = RuntimeAsset->LoadStaticMesh(0, StaticMeshConfig);
        if (StaticMesh)
        {
            auto LoadInfo = FLoadInfo();
            LoadInfo.Name = ModelName;
            LoadInfo.Collision = Collision;
            LoadInfo.StaticMesh = StaticMesh;
            LoadInfoArray.Add(LoadInfo);
            
            StaticMeshComponent->SetStaticMesh(StaticMesh);
            return true;
        }
    }

    return false;
}