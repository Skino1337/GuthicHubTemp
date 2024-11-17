#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Components/TextRenderComponent.h"

#include "GHWaypoint.generated.h"


USTRUCT(BlueprintType)
struct FWaypoint
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 WaterDepth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool UnderWater;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FVector Position;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FRotator Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FString> Links;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Previous;

	FWaypoint()
	{
		Name = "";
		WaterDepth = 0;
		UnderWater = false;
		Position = FVector::ZeroVector;
		Direction = FRotator::ZeroRotator;
		Links = {};
		Previous = "";
	}

	bool operator==(const FWaypoint& Rhs) const
	{
		return Name == Rhs.Name;
	}
};

UCLASS()
class GOTHICHUB_API AGHWaypoint : public AActor
{
	GENERATED_BODY()
	
public:
	AGHWaypoint();
	
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
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
};