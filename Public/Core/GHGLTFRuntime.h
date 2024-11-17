// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "glTFRuntimeFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"

#include "GHGLTFRuntime.generated.h"


USTRUCT(BlueprintType)
struct FLoadInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool Collision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UStaticMesh* StaticMesh;

	FLoadInfo()
	{
		Name = "";
		Collision = false;
		StaticMesh = nullptr;
	}
};


UCLASS()
class GOTHICHUB_API UGHGLTFRuntime : public UObject
{
	GENERATED_BODY()

public:
	UGHGLTFRuntime();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLoadInfo> LoadInfoArray;
	
	UFUNCTION()
	bool LoadModel(UStaticMeshComponent* StaticMeshComponent, FString ModelName, bool Collision);

	UFUNCTION()
	bool LoadModelYF(UStaticMeshComponent* StaticMeshComponent, FString ModelName, bool Collision);

	UFUNCTION()
	static FString PrepareFilePath(const FString& RelativePath);
	
	UFUNCTION()
	static USkeletalMesh* LoadSkeletalMesh(const FString& FileName, USkeleton* Skeleton);

	UFUNCTION()
	static USkeletalMesh* LoadMorphedSkeletalMesh(const FString& PathToFile);

	UFUNCTION()
	static UAnimSequence* LoadAnimSequence(USkeletalMesh* SkeletalMesh, const FString& FileName);

	UFUNCTION()
	static UAnimSequence* LoadAnimSequenceForBlendSpace(USkeletalMesh* SkeletalMesh, const FString& FileName);
};
