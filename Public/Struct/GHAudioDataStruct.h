#pragma once


#include "CoreMinimal.h"

//

#include "Sound/SoundWaveProcedural.h"

//

#include "GHAudioDataStruct.generated.h"


UENUM()
enum class ECategory: int32
{
	Other					= 0,
	Effect					= 1,
	Dialog					= 2,
	Music					= 3,
	Environments			= 4
};

USTRUCT(BlueprintType)
struct FAudioFile
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UObject* ImportedSoundWave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Volume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int Weight;

	FAudioFile()
	{
		Name = "";
		ImportedSoundWave = nullptr;
		Volume = 1.0f;
		Weight = 1000;
	}
};

USTRUCT(BlueprintType)
struct FAudioData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString RelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString FolderRelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bIsLoaded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float VolumeCoef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	ECategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FAudioFile> FileArray;

	FAudioData()
	{
		Name = "";
		RelativePath = "";
		FolderRelativePath = "";
		bIsLoaded = false;
		VolumeCoef = 1.0f;
		Category = ECategory::Other;
		Range = 2000;
		FileArray = {};
	}

	bool IsValid()
	{
		return !Name.IsEmpty() && bIsLoaded;
	}
};