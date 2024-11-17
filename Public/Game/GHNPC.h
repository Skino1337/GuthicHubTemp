// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"


#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "NavigationInvokerComponent.h"
#include "Navigation/PathFollowingComponent.h"




#include "Core/GHLuaState.h"

//#include "Game/GHDialog.h"

#include "..\Animations\GHMorphedSkeletalMeshComponent.h"

#include "Game/GHWaypoint.h"

#include "UI/GHEntityTooltip.h"
#include "Struct/GHDialogStruct.h"
#include "Struct/GHEnums.h"
#include "Struct/GHEventMessageStruct.h"

#include "LuaBlueprintFunctionLibrary.h"
#include "glTFRuntimeFunctionLibrary.h"


#include "Game/NPC/GHNPCActionQueue.h"
#include "Game/NPC/GHNPCDailyRoutine.h"



#include "GHNPC.generated.h"



UENUM()
enum class EAIState : int32
{
	None,
	Stop,
	Attack
};

UENUM()
enum class EControlledBy : int32
{
	None,
	Player,
	AI,
	Unknown
};

UENUM()
enum class EAnimationPreset : int32
{
	None,
	Walk,
	Run,
	Sneak,
	Weapon1H,
	Weapon2H,
	Bow,
	CrossBow,
	BowWalk,
	BowRun,
	BowSneak,
	CrossBowWalk,
	CrossBowRun,
	CrossBowSneak
};

UCLASS()
class GOTHICHUB_API AGHNPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGHNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when this Pawn is possessed. Only called on the server (or in standalone).
	virtual void PossessedBy(AController* NewController) override;
	
	// Called when our Controller no longer possesses us. Only called on the server (or in standalone).
	virtual void UnPossessed() override;

public:
	// Camera boom positioning the camera behind the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TMap<FString, USkeletalMeshComponent*> SkeletalMeshComponentMap;
	
	// Camera boom positioning the camera behind the character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// Follow camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// UStaticMeshComponent* MeshComponentSword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UStaticMeshComponent*> StaticMeshComponentMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UGHMorphedSkeletalMeshComponent*> MorphedMeshComponentMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponRightHandMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponLeftBeltMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UGHAnimInstance* AnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHEntityTooltip* Tooltip;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* TooltipComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNavigationInvokerComponent* NavigationInvoker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DebugString1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DebugString2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DebugString3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DebugString4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLuaValue _table;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomMinValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomMaxValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomOutDistanceForTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraZoomInDistanceForTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RollTurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RollTurnDegreesForTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YawTurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YawTurnDegreesForTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Visual;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EControlledBy ControlledBy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle StateLoopTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CurrentStateName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFightMode CurrentFightMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWalkMode WalkMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, EAttitude> AttitudeGuild;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, EAttitude> AttitudePerm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, EAttitude> AttitudeTemp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLuaValue TrueGuild;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GotoWaypoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGHNPC* GotoNpc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWaypoint> MoveWaypointArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDialog> Dialogs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWeaponLeftBeltInHands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TestProperty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIState State;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue CurrentWeaponLeftBelt;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue CurrentWeaponOnTheBack;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue CurrentWeaponInHands;

	// ^ to delete

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponSwapEquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponSwapHandSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLuaValue ItemInHands;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLuaValue EquippedMeleeWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLuaValue EquippedRangeWeapon;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TArray<UEventMessage*> ActionQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHNPCActionQueue* ActionQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UGHNPCDailyRoutine*> DailyRoutineArray;
	
	UFUNCTION()
	static void SetupSkeletonSockets(USkeletalMesh* SkeletalMesh, const TMap<FString, FSocketData>& SocketDataMap);

	UFUNCTION()
	void SetupStaticMeshComponents();
	
	UFUNCTION()
	void SetVisual(FString NewVisual);

	UFUNCTION()
	void SetBodyMesh(const FString& BodyMesh, int32 TextureV = -1);
	
	UFUNCTION()
	void DelayedBeginPlay();

	UFUNCTION()
	FLuaValue GetLuaValue(FString Field);

	UFUNCTION()
	void StartStateLoop(FString StateName, FLuaValue Start, FLuaValue Loop, FLuaValue End, FLuaValue Waypoint);

	UFUNCTION()
	void StateLoop(FLuaValue Loop, FLuaValue End, FLuaValue Waypoint);

	UFUNCTION()
	void SetWalkMode(EWalkMode NewWalkMode);

	UFUNCTION()
	FString GetMaterialNameUnderFoot();
	
	UFUNCTION()
	void PlayStepSound();
	
	UFUNCTION()
	void Attack();

	UFUNCTION()
	void SwapWeaponMesh(bool bIsDraw);
	
	UFUNCTION()
	void ReadyMeleeWeapon();

	UFUNCTION()
	void RemoveAnyWeapon();

	UFUNCTION()
	FLuaValue InventoryCreateItems(FLuaValue LuaValueItemInstance, int32 Count);

	// ---
	// Input
	// ---

	UFUNCTION()
	void InputMeleeWeaponToggle();

	UFUNCTION()
	void InputRangeWeaponToggle();

	UFUNCTION()
	void InputMagicWeaponToggle();
	
	// ---
	// Input
	// ---

	UFUNCTION()
	void RemoveWeaponLeftBelt();

	UFUNCTION()
	void EquipMesh(FLuaValue LuaValueWeapon);

	UFUNCTION()
	void SetHeadMesh(const FString& HeadName);

	UFUNCTION()
	void UnequipWeapon(FLuaValue LuaValueWeapon);

	UFUNCTION()
	void UseItemFromIndex(int32 Index);
	
	UFUNCTION()
	void ChangeTooltip(FString Text);

	UFUNCTION()
	void PrintInventory();
	
	UFUNCTION()
	void PrintDialogs();
	
	UFUNCTION()
	UGHNPCDailyRoutine* GetCurrentDailyRoutine();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void Test_LoadSkeletalMesh();

	UFUNCTION()
	void Test_LoadAnimation();

	UFUNCTION()
	void Test_PlayAnimation();
};
