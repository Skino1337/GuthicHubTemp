#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Struct/GHMorphDataStruct.h"

#include "GHMorphedSkeletalMeshComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(MorphAnim, Log, All);


UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GOTHICHUB_API UGHMorphedSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UGHMorphedSkeletalMeshComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMorphMeshData MorphData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMorphAnimData CurrentMorphAnimData;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 Layer;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float BlendIn;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float Hold;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float BlendOut;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString Flags;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString ASCName;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 FrameStart;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 FrameEnd;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float AnimationFramesPerSecond;

	//
	// ---
	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FrameCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FrameCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FrameWindow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayAnimation;

	//
	// ---
	//
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void PlayAnimationByName(const FString& AnimName);

	UFUNCTION()
	void StopAnimation();

	UFUNCTION()
	void RestartAnimation();

	UFUNCTION()
	void SetMorphData(const FMorphMeshData& InMorphData);
	
	// void TickComponent(float DeltaTime) override;
	
	// bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
};