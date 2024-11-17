#pragma once


#include "CoreMinimal.h"

#include "GHEventMessageStruct.generated.h"


// https://github.com/REGoth-project/REGoth-bs/blob/master/src/AI/EventMessage.hpp


UENUM()
enum class EActionType : int32
{
	Event,
	Npc,
	Damage,
	Weapon,
	Movement,
	Attack,
	UseItem,
	State,
	Manipulate,
	Conversation,
	Magic,
	Mob,

	SndPlay,
	SndStarted,
	SndStop
};

UENUM()
enum class EActionStatus : int32
{
	Add,
	Execute,
	End,
	Wait
};

UENUM()
enum class EMessageDamage : int32
{
	None,
	Once,
	PerFrame,
	Max
};

UENUM()
enum class EMessageWeapon : int32
{
	None,
	DrawWeaponMelee,
	DrawWeaponRange,
	DrawWeaponMagic,
	RemoveWeapon,
	// RemoveWeapon2,
	// RemoveWeapon3,
	ChooseWeapon,
	ForceRemoveWeapon,
	Attack,
	EquipBestWeapon,
	EquipBestArmor,
	UnequipWeapons,
	UnequipArmor,
	EquipArmor,
	WeaponMax
};

UENUM()
enum class EMessageMovement : int32
{
	None,
	RobustTrace,
	GotoPos,
	GotoObject,
	GoRoute,
	Turn,
	TurnToPos,
	TurnToObject,
	TurnAway,
	Jump,
	SetWalkMode,
	WhirlAround,
	Standup,
	CanSeeNpc,
	Strafe,
	GotoFP,
	Dodge,
	BeamTo,
	AlignToFP,
	MoveMax
};

UENUM()
enum class EMessageAttack : int32
{
	None,
	AttackForward,
	AttackLeft,
	AttackRight,
	AttackRun,
	AttackFinish,
	Parade,
	AimAt,
	ShootAt,
	StopAim,
	Defend,
	AttackBow,
	AttackMagic,
	AttackMax
};

UENUM()
enum class EMessageUseItem : int32
{
	None,
	Drink,
	EquipItem,
	UnequipItem,
	UseItemMax
};

UENUM()
enum class EMessageState : int32
{
	None,
	StartState,
	Wait,
	SetNpcsToState,
	SetTime,
	ApplyTimedOverlay,
	StateMax
};

UENUM()
enum class EMessageManipulate : int32
{
	None,
	TakeObject,
	DropObject,
	ThrowObject,
	Exchange,
	UseMob,
	UseItem,
	InsertInteractItem,
	RemoveInteractItem,
	CreateInteractItem,
	DestroyInteractItem,
	PlaceInteractItem,
	ExchangeInteractItem,
	UseMobWithItem,
	CallScript,
	EquipItem,
	UseItemToState,
	TakeMob,
	DropMob,
	ManipMax
};

UENUM()
enum class EMessageConversation : int32
{
	None,
	PlayAniSound,
	PlayAni,
	PlaySound,
	LookAt,
	Output,
	OutputMonolog,
	OutputEnd,
	Cutscene,
	WaitTillEnd,
	Ask,
	WaitForQuestion,
	StopLookAt,
	StopPointAt,

	PointAt,
	QuickLook,
	PlayAni_NoOverlay,
	PlayAni_Face,

	ProcessInfos,
	StopProcessInfos,
	OutputSVM_Overlay,
	SndPlay,
	SndPlay3d,
	PrintScreen,
	StartFx,
	StopFx,
	ConvMax
};

UENUM()
enum class EConversationStatus : int32
{
	INIT,
	PLAYING,
	FADING_OUT
};

UENUM()
enum class EMessageMagic : int32
{
	None,
	Open,
	Close,
	Move,
	Invest,
	Cast,
	SetLevel,
	ShowSymbol,
	SetFrontSpell,
	CastSpell,
	Ready,
	Unready,
	MagicMax
};

UENUM()
enum class EMessageMob : int32
{
	None,
	StartInteraction,
	StartSTateChange,
	EndInteraction,
	Unlock,
	Lock,
	CallScript
};


UCLASS()
class UEventMessage : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EActionType MessageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EActionStatus MessageStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString MessageInformation;
	
	// Whether this should wait for other messages to complete before triggering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bJob;

	// Flag used for cleanup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bDeleted;

	// Whether this should overwrite existing messages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bHighPriority;

	// Whether this is currently playing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bInUse;

	// Whether this message should not block further execution of the message-queue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bOverlay;

	// Whether this is the first time this message is being handled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool bFirstRun;

	UEventMessage()
	{
		MessageType = EActionType::Event;
		MessageStatus = EActionStatus::Wait;
		MessageInformation = "";
		
		bJob = false;
		bDeleted = false;
		bHighPriority = false;
		bInUse = false;
		bOverlay = false;
		bFirstRun = true;
	}
};

UCLASS()
class UNpcMessage : public UEventMessage
{
	GENERATED_BODY()

public:
	UNpcMessage()
	{
		MessageType = EActionType::Npc;
		bJob = true;
	}
};

UCLASS()
class UWeaponMessage : public UNpcMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EMessageWeapon MessageWeapon;
	
	UWeaponMessage()
	{
		MessageType = EActionType::Weapon;

		MessageWeapon = EMessageWeapon::None;
	}
};

UCLASS()
class UUseItemMessage : public UNpcMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EMessageUseItem MessageUseItem;

	// what we do if two same item, like sword?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString InstanceItem;
	
	UUseItemMessage()
	{
		MessageType = EActionType::UseItem;

		MessageUseItem = EMessageUseItem::None;
		InstanceItem = "";
	}
};


UCLASS()
class UMovementMessage : public UNpcMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	EMessageMovement MessageMovement;
	
	// Route the NPC has to go if this is a GoRoute
	// If empty, targetPos is used
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	// TArray<HWaypoint> Route;
	
	// Object we are going to if this is a GotoVob
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	// bs::HSceneObject targetObject;
	
	// Position we shall go to, if this is a GotoPos or route and name are emtpy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FVector TargetPosition;
	
	// How we should go to our position
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	// AI::WalkMode walkMode = AI::WalkMode::Run;
	
	// General purpose mode
	int32 TargetMode;
	
	UMovementMessage()
	{
		MessageType = EActionType::Movement;
		
		MessageMovement = EMessageMovement::None;
		TargetPosition = FVector{0.0f, 0.0f, 0.0f};
		TargetMode = 0;
	}
};

//
// USTRUCT(BlueprintType)
// struct FEventMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EActionType MessageType;
//
// 	// Whether this should wait for other messages to complete before triggering
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	bool bJob;
//
// 	// Flag used for cleanup
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	bool bDeleted;
//
// 	// Whether this should overwrite existing messages
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	bool bHighPriority;
//
// 	// Whether this is currently playing
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	bool bInUse;
//
// 	// Whether this message should not block further execution of the message-queue
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	bool bOverlay;
//
// 	// Whether this is the first time this message is being handled
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	bool bFirstRun;
//
// 	FEventMessage()
// 	{
// 		MessageType = EActionType::Event;
// 		bJob = false;
// 		bDeleted = false;
// 		bHighPriority = false;
// 		bInUse = false;
// 		bOverlay = false;
// 		bFirstRun = true;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FNpcMessage : public FEventMessage
// {
// 	GENERATED_BODY()
// 	
// 	FNpcMessage()
// 	{
// 		MessageType = EActionType::Npc;
// 		bJob = true;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FDamageMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageDamage MessageDamage;
// 	
// 	FDamageMessage()
// 	{
// 		MessageType = EActionType::Damage;
// 		
// 		MessageDamage = EMessageDamage::None;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FWeaponMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageWeapon MessageWeapon;
//
// 	// // Type of weapon to go to 
// 	// WeaponMode targetMode;
// 	
// 	FWeaponMessage()
// 	{
// 		MessageType = EActionType::Weapon;
// 		
// 		MessageWeapon = EMessageWeapon::None;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FMovementMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageMovement MessageDamage;
// 	
// 	// Route the NPC has to go if this is a GoRoute
// 	// If empty, targetPos is used
// 	// TArray<HWaypoint> Route;
// 	
// 	// Object we are going to if this is a GotoVob
// 	// bs::HSceneObject targetObject;
// 	
// 	// Position we shall go to, if this is a GotoPos or route and name are emtpy
// 	// bs::Vector3 targetPosition;
// 	
// 	// How we should go to our position 
// 	// AI::WalkMode walkMode = AI::WalkMode::Run;
// 	
// 	// General purpose mode 
// 	// int targetMode = 0;
// 	
// 	FMovementMessage()
// 	{
// 		MessageType = EActionType::Movement;
// 		
// 		MessageDamage = EMessageMovement::None;
// 	}
// };

// USTRUCT(BlueprintType)
// struct FAttackMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageAttack MessageDamage;
// 	
// 	// How far we are in our combo
// 	// bs::INT32 combo;
//
// 	// Object we are attacking
// 	// bs::HSceneObject targetObject;
//
// 	// Animation to use when attacking, -1 if none.
// 	// bs::INT32 animationIndex;
//
// 	// Frame to skip the animation to 
// 	// float startFrame;
// 	
// 	FAttackMessage()
// 	{
// 		MessageType = EActionType::Attack;
// 		
// 		MessageDamage = EMessageAttack::None;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FUseItemMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageUseItem MessageDamage;
// 	
// 	// How far we are in our combo
// 	// bs::INT32 combo;
//
// 	// Object we are attacking
// 	// bs::HSceneObject targetObject;
//
// 	// Animation to use when attacking, -1 if none.
// 	// bs::INT32 animationIndex;
//
// 	// Frame to skip the animation to 
// 	// float startFrame;
// 	
// 	FUseItemMessage()
// 	{
// 		MessageType = EActionType::UseItem;
// 		
// 		MessageDamage = EMessageUseItem::None;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FStateMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageState MessageDamage;
// 	
// 	//Name of the Setupfunction for this state (ZS_...)
// 	//bs::String state;
// 	
// 	//Whether the old state should be ended properly, or be interrupted and canceled.
// 	//bool interruptOldState = false;
// 	
// 	//Symbols for other and victim
// 	//HCharacter other;
// 	//HCharacter victim;
// 	
// 	//Whether this belongs to the daily routine
// 	//bool isRoutineState = false;
// 	
// 	//Whether this is a program-managed state. List of states in NpcScriptState.h
// 	//bool isPrgState = false;
// 	
// 	//If this is a wait-message, this is how long we should wait
// 	//float waitTime = 0.0f;
// 	
// 	//Waypoint name to got to, in case the state needs that
// 	//bs::String wpname;
// 	
// 	FStateMessage()
// 	{
// 		MessageType = EActionType::State;
// 		
// 		MessageDamage = EMessageState::None;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FManipulateMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageManipulate MessageManipulate;
// 	
// 	//  Symbol of the item to use. If no item with this symbol can be found in the inventory,
// 	//  nothing happens Alternatively, you can set "targetItem" to a valid handle.
// 	//  bs::String itemInstance;
// 		
// 	//  Handle of the item to use. Alternatively you could set "name" to something found in the inventory
// 	//  HItem targetItem;
//
// 	//  Slot the item should be put in, in case this is a *InteractItem.
// 	//  FIXME: What is a slot?
// 	//  bs::String slot;
// 		
// 	//  General purpose flag
// 	//  bool flag;
// 		
// 	//  Y-coord of the the pickup animation?
// 	//  float pickupAniY;
// 		
// 	//  State to go to when using the item
// 	//  bs::INT32 targetState;
// 		
// 	//  Animation to play, for example on pickup
// 	//  bs::String animation;
// 	
// 	FManipulateMessage()
// 	{
// 		MessageType = EActionType::Manipulate;
// 		
// 		MessageManipulate = EMessageManipulate::None;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FConversationMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageConversation MessageConversation;
// 	
// 	// Text to be displayed in the subtitle box
// 	// bs::String text;
// 		
// 	// Name of the SFX to play
// 	// bs::String name;
//
// 	// Target-object to look at
// 	// bs::HSceneObject target;
// 		
// 	// Target-pos to look at
// 	// bs::Vector3 targetPosition;
// 		
// 	// Only valid while the message is being processed. Clip matching the given animation name.
// 	// HZAnimationClip playingClip;
// 		
// 	// Animation name to be used. If empty, index will be checked.
// 	// bs::String animation;
// 		
// 	// Stores the state of the message, to handle multiple stages/states inside the Playercontroller
// 	// EConversationStatus Status;
// 		
// 	// Whether this message has been kindly canceled by the ouside (i.e. DialogManager).
// 	// bool canceled;
// 		
// 	// Ticket. Can be used to ask AudioWorld if sound is playing.
// 	// FIXME: Reimplement that using a sound handle
// 	// Utils::Ticket<World::AudioWorld> soundTicket;
// 	
// 	FConversationMessage()
// 	{
// 		MessageType = EActionType::Conversation;
// 		
// 		MessageConversation = EMessageConversation::None;
// 		// status      = EConversationStatus::INIT;
// 		// canceled    = false;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FMagicMessage : public FNpcMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageMagic MessageMagic;
// 	
// 	FMagicMessage()
// 	{
// 		MessageType = EActionType::Magic;
// 		
// 		MessageMagic = EMessageMagic::None;
// 	}
// };
//
// USTRUCT(BlueprintType)
// struct FMobMessage : public FEventMessage
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	EMessageMob MessageMob;
//
// 	// NPC this message is from */
// 	// bs::HSceneObject npc;
// 		
// 	// State to change from.
// 	int32 StateFrom;
// 		
// 	// State to change to.
// 	int32 StateTo;
//
// 	// Whether to play an animation.
// 	bool PlayAnimation;
// 	
// 	FMobMessage()
// 	{
// 		MessageType = EActionType::Mob;
// 		
// 		MessageMob = EMessageMob::None;
// 		StateFrom = 0;
// 		StateTo = 0;
// 		PlayAnimation = false;
// 	}
// };