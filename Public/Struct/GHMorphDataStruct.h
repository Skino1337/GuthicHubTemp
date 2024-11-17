#pragma once


#include "CoreMinimal.h"

#include "GHMorphDataStruct.generated.h"


USTRUCT(BlueprintType)
struct FMorphAnimData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 Layer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float BlendIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Hold;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float BlendOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Flags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString ASCName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 FrameStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 FrameEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 Speed;

	FMorphAnimData()
	{
		Name = "";
		Layer = 1;
		BlendIn = 0.0f;
		Hold = -1;
		BlendOut = 0.0f;
		Flags = "";
		ASCName = "";
		FrameStart = 0;
		FrameEnd = 1;
		Speed = 25;
	}

	bool IsValid()
	{
		return !Name.IsEmpty();
	}
};


USTRUCT(BlueprintType)
struct FMorphMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString RelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TMap<FString, FMorphAnimData> MorphAnimDataMap;

	FMorphMeshData()
	{
		Name = "";
		Path = "";
		RelativePath = "";
		SkeletalMesh = nullptr;
		MorphAnimDataMap = {};
	}

	bool IsValid()
	{
		return !Name.IsEmpty() && SkeletalMesh != nullptr;
	}
};