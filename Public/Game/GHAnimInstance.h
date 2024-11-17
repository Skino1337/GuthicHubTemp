// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/KismetStringLibrary.h"

#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimNodeReference.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"

//#include "AnimationBlueprintLibrary.h"
//#include "/Engine/Source/Editor/AnimationModifiers/Public/AnimationBlueprintLibrary.h"

#include "Animation/AimOffsetBlendSpace.h"
#include "Animation/AimOffsetBlendSpace1D.h"
#include "Animation/AnimSequence.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Core/GHGameInstance.h"


#include "Game/GHGameMode.h"
#include "Game/GHNPC.h"
#include "Game/GHDebug.h"

#include "GHAnimInstance.generated.h"

// to self ref
//typedef struct FAnimMontageData FAnimMontageData;

DECLARE_LOG_CATEGORY_EXTERN(SkAnimation, Log, All);

USTRUCT(BlueprintType)
struct FAnimationHistory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bRootMotion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bInterrupted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bFinished;

	FAnimationHistory()
	{
		Name = "";
		bRootMotion = true;
		Duration = 0.0f;
		bInterrupted = false;
		bFinished = false;
	}
};

UENUM()
enum class EBodyState : int32
{
	None,
	Stand,
	Walk,
	Run
};


UCLASS()
class GOTHICHUB_API UGHAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UGHAnimInstance();

private:
	UFUNCTION()
	void FunctionToExecuteOnAnimationStarted(UAnimMontage* Montage);
	
	UFUNCTION()
	void FunctionToExecuteOnAnimationEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(EditAnywhere)
	AGHGameMode* GameMode;
	
protected:
	virtual void NativeInitializeAnimation() override;

	void DelayedInit();

	virtual void NativeUpdateAnimation(float DeltaSeconds);

public: // protected
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> AnimSequenceIdle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> AnimSequenceMoveForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AnimMontageIdle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AnimMontageMoveForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> MontageArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* AnimationTest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> MontageTest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMeshTest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeleton* SkeletonTest;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	TObjectPtr<UAimOffsetBlendSpace> BowAimOffsetBlendSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	TObjectPtr<UAnimSequence> AnimSequenceBowAimUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	TObjectPtr<UAnimSequence> AnimSequenceBowAimForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	TObjectPtr<UAnimSequence> AnimSequenceBowAimDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	TObjectPtr<UAnimSequence> AnimSequenceBowAimUpLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	TObjectPtr<UAnimSequence> AnimSequenceBowAimForwardLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	TObjectPtr<UAnimSequence> AnimSequenceBowAimDownLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	float BowAimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GothicHub)
	float BowAimYaw;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AnimationGroupName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGHNPC* Npc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRunStateMachine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoveForwardPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoveBackwardPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoveLeftPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoveRightPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTurnLeftPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTurnRightPressed;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString IdleAnimationName;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString MoveForwardAnimationName;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString MoveBackwardAnimationName;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString MoveLeftAnimationName;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString MoveRightAnimationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCharacterMovementComponent* CharacterMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime AnimationStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationElapsedTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOnAnimate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBodyState BodyState;

	// DON'T USE SMART POINTERS, GC RECTED
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationData* IdleAnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationData* MoveForwardAnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationData* MoveBackwardAnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationData* MoveLeftAnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationData* MoveRightAnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationData* TurnLeftAnimationData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationData* TurnRightAnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAnimationHistory> AnimationHistoryArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimationData*> AnimationDataQueueArray;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FAnimationData PreviousAnimationData;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FAnimationData CurrentAnimationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UAnimationData*> CurrentAnimationDataLayerMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UAnimationData*> PreviousAnimationDataLayerMap;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TMap<FString, FAnimationData> AnimationDataLayerLayerMap;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UAnimMontage* PreviousMontage;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UAnimMontage* CurrentMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PreviousMontagePosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentMontagePosition;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString CurrentMontageName;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString CurrentAnimationName;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString NextAnimationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationRemainSecondsWithBlend;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FAnimationData CurrentAnimationData;

	UFUNCTION()
	void SetupBlendSpaces();

	UFUNCTION()
	static void SetupSkeletonSlots(USkeleton* Skeleton);

	UFUNCTION()
	void OnMontageChangeSection();

	UFUNCTION()
	void OnEventTrigged(const FEventData& EventData);

	UFUNCTION()
	void AddAnimNotifyToAnimMontage(UAnimMontage* AnimMontage);

	UFUNCTION()
	static bool CanNotifyUseTrack(const FAnimNotifyTrack& Track, const FAnimNotifyEvent& Notify);
	
	UFUNCTION()
	static FAnimNotifyTrack& AddNewTrack(TArray<FAnimNotifyTrack>& Tracks);

	void UpdateBodyState();


	
	UFUNCTION()
	static void AddAnimNotifyToAnimSequence(UAnimSequence* AnimSequence);
	
	UFUNCTION()
	FString CheckHardCodeAnimation(const FString& Name);

	UFUNCTION()
	void AnimationHistoryAdd(FString& Name, bool bRootMotion);

	UFUNCTION()
	void AnimationHistoryRemove();
	
	UFUNCTION()
	void AnimationHistoryEnd(UAnimationData* AnimationData, bool bInterrupted);

	UFUNCTION()
	void AnimationHistoryUpdate(float Elapsed);
	
	UFUNCTION()
	static int32 AnimMontageAddSection(UAnimMontage* AnimMontage, FName InSectionName, float StartTime, FName NextSectionName = NAME_None);

	UFUNCTION()
	bool SetupAnimationData(UAnimationData* AnimationData, bool bRootMotion = true);
	
	UFUNCTION()
	static UAnimMontage* CreateAnimMontageFromAnimSequence(UAnimSequence* AnimSequence, UAnimationData* AnimationData);

	UFUNCTION()
	void UpdateAnimationPreset(EAnimationPreset EAnimationPreset = EAnimationPreset::None, bool Update = true);
	
	// UFUNCTION()
	// static bool Montage_SetNextSection(const UAnimMontage* Montage, FName SectionNameToChange, FName NextSection);

	UFUNCTION()
	FString ComputeTransitionAnimation(const FString& AnimationNameFrom, const FString& AnimationNameTo);

	UFUNCTION()
	FString GetTransitionTargetAnimationName(const FString& AnimationName);

	UFUNCTION()
	UAnimSequence* GetAnimSequenceByName(const FString& AnimationName);

	UFUNCTION()
	UAnimMontage* GetAnimMontageByName(const FString& AnimationName, bool bRootMotion = true);
	
	UFUNCTION()
	UAnimationData* GetAnimationDataByName(const FString& AnimationName);

	UFUNCTION()
	bool IsMontagePlayed(const FString& AnimationName);

	UFUNCTION()
	void StopAllMontagesByLayer(const FString& Layer);

	UFUNCTION()
	void StopMontage(UAnimationData* AnimationData, float BlendOut = 0.0f);
	
	UFUNCTION()
	float PlayMontage(UAnimationData* AnimationData, bool bRootMotion = true, bool bPlaySame = false, bool bPlayTransitionInstead = true);

	UFUNCTION()
	float PlayMontageByName(const FString& AnimationName, bool bRootMotion = true, bool bPlaySame = false, bool bPlayTransitionInstead = true);

	UFUNCTION()
	bool PlayMontageQueue(TArray<FString> AnimationNameArray);
	
	UFUNCTION()
	void PlayMontageByNameOld(const FString& MontageName);

	UFUNCTION()
	void PlayMontageTest();

	UFUNCTION()
	void PlayMontageTest2();

	UFUNCTION()
	void StopAllMontage(float InBlendOutTime);

	UFUNCTION()
	void RunStateMachine();

	UFUNCTION()
	void ResetStateMachine(const FString& EndedAnimationName = "");

	UFUNCTION()
	void UpdateStateMachine(float DeltaSeconds = 0.0f, bool bInvokedFromAnimationFinish = false);

	UFUNCTION()
	void AnimationFinishStateMachine(UAnimationData* AnimationData, bool bInterrupted);

	UFUNCTION()
	void ChooseMoveDirection(bool bPlaySame = false);

	UFUNCTION()
	void StartMoveForward();

	UFUNCTION()
	void StopMoveForward();
	
	UFUNCTION()
	void StartMoveBackward();

	UFUNCTION()
	void StopMoveBackward();

	UFUNCTION()
	void StartMoveLeft();

	UFUNCTION()
	void StopMoveLeft();
	
	UFUNCTION()
	void StartMoveRight();

	UFUNCTION()
	void StopMoveRight();

	UFUNCTION()
	void ChooseRotateDirection();
	
	UFUNCTION()
	void StartTurnLeft();

	UFUNCTION()
	void StopTurnLeft();

	UFUNCTION()
	void StartTurnRight();

	UFUNCTION()
	void StopTurnRight();
};
