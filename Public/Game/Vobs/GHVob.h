#pragma once


#include "CoreMinimal.h"

#include "Components/TextRenderComponent.h"

#include "GHVob.generated.h"


USTRUCT(BlueprintType)
struct FVobStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Class;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 Pack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString PresetName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<float> BBox3DWS;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FRotator TrafoOSToWSRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FRotator> RotArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FVector TrafoOSToWSPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString VobName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Visual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool ShowVisual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 VisualCamAlign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool CdStatic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool CdDyn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool StaticVob;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 DynShadow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 ZBias;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool IsAmbient;
	
	FVobStruct()
	{
		Class = "";
		Pack = 0;
		PresetName = "";
		BBox3DWS = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		TrafoOSToWSRot = FRotator::ZeroRotator;
		TrafoOSToWSPos = FVector::ZeroVector;
		VobName = "";
		Visual = "";
		ShowVisual = false;
		VisualCamAlign = 0;
		CdStatic = true;
		CdDyn = true;
		StaticVob = true;
		DynShadow = false;
		ZBias = 0;
		IsAmbient = false;
	}
};


UCLASS()
class GOTHICHUB_API AGHVob : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AGHVob();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextRenderComponent> NameTextRenderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextRenderComponent> PosTextRenderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTextRenderComponent> DirTextRenderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVobStruct ZenStructVob;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
