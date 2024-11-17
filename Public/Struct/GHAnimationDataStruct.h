#pragma once


#include "CoreMinimal.h"

#include "GHAnimationDataStruct.generated.h"


USTRUCT(BlueprintType)
struct FEventData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Time;

	// eventSFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Slot;

	// eventPFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 Handle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Node;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Attach;

	// eventMMStartAni
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Animation;
	
	// eventTag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Tag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Arg1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Arg2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Arg3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Arg4;
	
	FEventData()
	{
		Name = "";
		Time = 0.0f;

		// eventSFX
		Effect = "";
		Radius = 0.0f;
		Slot = "";

		// eventPFX
		Handle = 0;
		// Effect = "";
		Node = "";
		Attach = "";
		
		// eventMMStartAni
		Animation = "";

		// eventTag
		Tag = "";
		Arg1 = "";
		Arg2 = "";
		Arg3 = "";
		Arg4 = "";
	}
};

USTRUCT(BlueprintType)
struct FSocketData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Bone;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FRotator Rotation;

	FSocketData()
	{
		Name = "";
		Bone = "";
		Position = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
	}
};

// https://www.reddit.com/r/cpp_questions/comments/feb1ss/initializing_variables_in_header_vs_source_file/

UCLASS(BlueprintType)
class UAnimationData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UAnimSequence* AnimSequence;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UAnimSequence* AnimSequenceNoRootMotion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	UAnimMontage* AnimMontageNoRootMotion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FString> BlendBones;
 	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Layer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString NextAnimation;
 	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float BlendIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float BlendOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Flags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString AscName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bIsReverse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float EndTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	int32 FPS;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FEventData> Events;
	
	UAnimationData()
	{
		AnimSequence = nullptr;
		AnimMontage = nullptr;
		AnimSequenceNoRootMotion = nullptr;
		AnimMontageNoRootMotion = nullptr;
		BlendBones = {};
		Name = "";
		Layer = "1";
		NextAnimation = "";
		BlendIn = 0.0f;
		BlendOut = 0.0f;
		Flags = "M";
		AscName = "";
		bIsReverse = false;
		StartTime = 0.0f;
		EndTime = 0.0f;
		FPS = 25;
		Events = {};
	}

	bool IsValid()
	{
		return AnimSequence != nullptr && !Name.IsEmpty();
	}

	bool operator==(const UAnimationData& Rhs) const
	{
		return Name == Rhs.Name;
	}
	
	bool operator!=(const UAnimationData& Rhs) const
	{
		return Name != Rhs.Name;
	}
	
	bool operator==(const FString& Rhs) const
	{
		return Name == Rhs;
	}

	bool operator!=(const FString& Rhs) const
	{
		return Name != Rhs;
	}
	
	// bool operator==(const FAnimationData& Lhs, const FAnimationData& Rhs)
	// {
	// 	return Lhs.Name == Rhs.Name;
	// }

	// inline bool operator==(const FAnimationData& Lhs, const FAnimationData& Rhs)
	// {
	// 	return Lhs.Name == Rhs.Name;
	// }
};

USTRUCT(BlueprintType)
struct FAnimationGroupData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString GroupName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString RelativePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString SkeletonFileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	USkeleton* SharedSkeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	USkeletalMesh* SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TMap<FString, FSocketData> SocketDataMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TArray<FString> SupportedMeshArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	TMap<FString, UAnimationData*> AnimationDataMap;

	FAnimationGroupData()
	{
		GroupName = "";
		RelativePath = "";
		SkeletonFileName = "";
		SharedSkeleton = nullptr;
		SkeletalMesh = nullptr;
		SocketDataMap = {};
		SupportedMeshArray = {};
		AnimationDataMap = {};
	}
};