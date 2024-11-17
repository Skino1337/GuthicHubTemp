// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Game/Vobs/GHVob.h"

#include "GHVobLight.generated.h"

UENUM()
enum class ELightType: int32
{
	Point				= 0,	// The light source emits a cone-shaped light beam.
	Spot				= 1,	// The light source emits light in a sphere.
	Reserved0			= 2,
	Reserved1			= 3
};

UENUM()
enum class ELightQuality: int32
{
	High				= 0,	// The light is a high quality light.
	Medium				= 1,	// The light is a medium quality light.
	Low					= 2		// The light is a low quality light, which should be the fastest option.
};


USTRUCT(BlueprintType)
struct FVobLightStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FVobStruct VobStruct;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString LightPresetInUse;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	ELightType LightType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Range;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FColor Color;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float SpotConeAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool LightStatic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	ELightQuality LightQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString LensFlareFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool TurnedOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float RangeAniScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float RangeAniFPS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool RangeAniSmooth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FColor> ColorAniList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float ColorAniFPS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool ColorAniSmooth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool CanMove;
	
	FVobLightStruct()
	{
		LightPresetInUse = "";
		LightType = ELightType::Spot;
		Range = 1000.0f;
		Color = {255, 0, 0, 255};
		SpotConeAngle = 180.0f;
		LightStatic = true;
		LightQuality = ELightQuality::High;
		LensFlareFX = "";

		TurnedOn = true;
		RangeAniScale = 0.0f;
		RangeAniFPS = 1.0f;
		RangeAniSmooth = true;
		ColorAniList = {};
		ColorAniFPS = 1.0f;
		ColorAniSmooth = true;
		CanMove = false;
	}
};

UCLASS()
class GOTHICHUB_API AGHVobLight : public AGHVob
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVobLightStruct ZenStructVobLight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
