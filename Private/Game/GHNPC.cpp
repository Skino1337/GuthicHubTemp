// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHNPC.h"

#include "AIController.h"
#include "ImageUtils.h"
#include "Game/GHAnimInstance.h"
#include "MeshAttributes.h"
#include "AI/GHAIController.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Core/GHAssetManager.h"
#include "Core/GHGameInstance.h"

#include "Game/GHGameMode.h"

#include "Game/NPC/GHNPCActionQueue.h"

#include "UI/Quest/GHQuestPanel.h"


// Sets default values
AGHNPC::AGHNPC()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Configure capsule component
	if (auto CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->InitCapsuleSize(42.f, 96.0f);
	}

	// Configure mesh component
	auto SkeletalMeshComp = GetMesh();
	if (SkeletalMeshComp)
	{
		SkeletalMeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		SkeletalMeshComp->SetVisibility(false);

		for (int32 i = 1; i <= 3; i++)
		{
			auto Name = FString::Printf(TEXT("Mesh%d"), i);
			auto SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName(*Name));
			if (SkeletalMeshComponent)
			{
				SkeletalMeshComponent->SetupAttachment(SkeletalMeshComp);
		
				SkeletalMeshComponentMap.Add(Name, SkeletalMeshComponent);
			}
		}
	}

	// Configure movement component
	if (auto MovementComp = GetCharacterMovement())
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
		MovementComp->JumpZVelocity = 700.f;
		MovementComp->AirControl = 0.35f;
		MovementComp->MaxWalkSpeed = 1000.f;
		MovementComp->MinAnalogWalkSpeed = 20.f;
		MovementComp->BrakingDecelerationWalking = 2000.f;
	}

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation(FVector(100.0f, 0.0f, 80.0f));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	// Create a tooltip
	TooltipComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Tooltip"));
	TooltipComponent->SetupAttachment(RootComponent);
	TooltipComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TooltipComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

	static ConstructorHelpers::FClassFinder<UUserWidget> TooltipClass(TEXT("/Game/UI/EntityTooltip"));
	if (TooltipClass.Class != nullptr)
		TooltipComponent->SetWidgetClass(TooltipClass.Class);

	// Create a Navigation Invoker
	NavigationInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvoker"));
	NavigationInvoker->SetGenerationRadii(2000.0f, 6000.0f);

	// socked: https://forums.unrealengine.com/t/can-you-add-a-socket-to-a-skeleton-using-c/211404/3
	// // Create Meshes
	// if (auto SkeletalMesh = GetMesh())
	// {
	// 	MeshComponentSword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	// 	MeshComponentSword->RegisterComponent();
	// 	this->AddInstanceComponent(MeshComponentSword);
	// 	MeshComponentSword->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepWorldTransform, m_name);
	// }
	


	//->AttachToComponent(SkeletalMeshComponent, AttachmentTransformRules, "weapon_belt_left");
	
	//WeaponRightHandMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Weapon Right Hand Mesh"));
	//WeaponRightHandMeshComponent->RegisterComponent();
	//this->AddInstanceComponent(WeaponRightHandMeshComponent);
	//WeaponRightHandMeshComponent->SetupAttachment(RootComponent);
	// WeaponRightHandMeshComponent->RegisterComponent();
	// UInstancedStaticMeshComponent* Item = CreateDefaultSubobject<UStaticMeshComponent>("text");
	
	//WeaponRightHandMeshComponent->SetSkeletalMesh()

	//WeaponBeltLeftMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Weapon Belt Left Mesh"));
	//WeaponBeltLeftMeshComponent->RegisterComponent();
	//this->AddInstanceComponent(WeaponBeltLeftMeshComponent);
	//WeaponBeltLeftMeshComponent->SetupAttachment(RootComponent);
	// WeaponBeltLeftMeshComponent->RegisterComponent();
	
	//UStaticMeshComponent* Wbl = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("MeshTest")));

	//WeaponLeftBeltMeshComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("MeshTest")));
	
	// AIControllerClass = AGruxAIController::StaticClass();
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AGHAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Disabled;

	MorphedMeshComponentMap = {};
	StaticMeshComponentMap = {};


	// toggle camdebug
	CameraZoomMinValue = CameraBoom->TargetArmLength;
	CameraZoomMaxValue = CameraZoomMinValue + 200.0f;
	CameraZoomOutDistanceForTime = (CameraZoomMaxValue - CameraZoomMinValue) / 0.8f;
	CameraZoomInDistanceForTime = (CameraZoomMaxValue - CameraZoomMinValue) / 0.6f;

	// capsule
	RollTurnSpeed = 15.0f;
	RollTurnDegreesForTime = RollTurnSpeed / 0.4f;
	// camera
	YawTurnSpeed = 15.0f;
	YawTurnDegreesForTime = YawTurnSpeed / 0.4f;

	DebugString1 = "";
	DebugString2 = "";
	DebugString3 = "";
	DebugString4 = "";
	
	_table = FLuaValue();

	Visual = "";
	IsPlayer = false;
	ControlledBy = EControlledBy::None;

	bIsWeaponLeftBeltInHands = false;
	// CurrentWeaponLeftBelt = FLuaValue();
	// CurrentWeaponOnTheBack = FLuaValue();
	// CurrentWeaponInHands = FLuaValue();
	
	CurrentStateName = "";
	CurrentStateTime = 0.0f;

	CurrentFightMode = EFightMode::None;
	WalkMode = EWalkMode::Walk;
	AttitudeGuild = {};
	AttitudePerm = {};
	AttitudeTemp = {};
	TrueGuild = FLuaValue();
	GotoWaypoint = "";
	GotoNpc = nullptr;
	// Escort?

	Dialogs = {};
	
	State = EAIState::None;
	TestProperty = "this is TestProperty";
	
	ActionQueue = CreateDefaultSubobject<UGHNPCActionQueue>("ActionQueue");
	// ActionQueue->NPC = this;
	DailyRoutineArray = {};
}

// auto SkeletalMeshComponent = GetMesh();
// if (!SkeletalMeshComponent)
// 	return;
//
// auto SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
// if (!SkeletalMesh)
// 	return;
//
// auto Skeleton = SkeletalMesh->GetSkeleton();
// if (!Skeleton)
// 	return;

void AGHNPC::SetupSkeletonSockets(USkeletalMesh* SkeletalMesh, const TMap<FString, FSocketData>& SocketDataMap)
{
	if (!SkeletalMesh)
		return;
	
	auto Skeleton = SkeletalMesh->GetSkeleton();
	if (!Skeleton)
		return;

	// UE_LOG(LogTemp, Warning, TEXT("shared skeleton pointer: %p"), Skeleton);
	
	Skeleton->Sockets.Empty();

	TArray<FString> BoneNameArray;
	for (int32 i = 0; i < SkeletalMesh->GetRefSkeleton().GetNum(); i++)
		BoneNameArray.Add(SkeletalMesh->GetRefSkeleton().GetBoneName(i).ToString());

	// for (auto BoneName : BoneNameArray)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "BoneName: " + BoneName);
	// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *BoneName);
	// }
	
	for (auto SocketDataTuple : SocketDataMap)
	{
		auto SocketData = SocketDataTuple.Value;
	
		if (!BoneNameArray.Contains(SocketData.Bone))
			continue;

		auto Socket = NewObject<USkeletalMeshSocket>(Skeleton);
		if (!Socket)
			continue;

		if (SocketData.Name.IsEmpty())
			continue;

		if (SocketData.Bone.IsEmpty())
			continue;
		
		Socket->SocketName = FName(SocketData.Name);
		Socket->BoneName = FName(SocketData.Bone);
		Socket->RelativeLocation = SocketData.Position;
		Socket->RelativeRotation = SocketData.Rotation;

		Skeleton->Sockets.Add(Socket);
	}

	if (Skeleton->Sockets.Num() != SocketDataMap.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Some socket not added"));
		return;
	}
}

// Called when the game starts or when spawned
void AGHNPC::BeginPlay()
{
	Super::BeginPlay();

	ActionQueue->NPC = this;

	auto World = GEngine->GetCurrentPlayWorld(nullptr);

	TooltipComponent->InitWidget();
	Tooltip = Cast<UGHEntityTooltip>(TooltipComponent->GetUserWidgetObject());
	
	//
	// auto SkeletalMeshComponent = GetMesh();  // FindComponentByClass<USkeletalMeshComponent>())
	// if (SkeletalMeshComponent && SkeletalMeshComponent->GetName().Contains("CharacterMesh"))
	// {
	// 	WeaponLeftBeltMeshComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("WeaponLeftBeltMeshComponent_fill")));
	// 	WeaponRightHandMeshComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("WeaponRightHandMeshComponent_fill")));
	// 	if (IsValid(WeaponLeftBeltMeshComponent) && IsValid(WeaponRightHandMeshComponent))
	// 	{
	// 		auto AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	// 		
	// 		WeaponLeftBeltMeshComponent->AttachToComponent(SkeletalMeshComponent, AttachmentTransformRules, "weapon_belt_left");
	// 		WeaponLeftBeltMeshComponent->AddLocalRotation(FRotator(0, 90, 90));
	// 		WeaponLeftBeltMeshComponent->SetVisibility(false);
	// 		
	// 		WeaponRightHandMeshComponent->AttachToComponent(SkeletalMeshComponent, AttachmentTransformRules, "hand_r_sword_socket");
	// 		WeaponRightHandMeshComponent->AddLocalRotation(FRotator(0, 270, 90));
	// 		WeaponRightHandMeshComponent->SetVisibility(false);
	// 	}
	// 	// if (IsValid(WeaponRightHandMeshComponent) && IsValid(WeaponBeltLeftMeshComponent))
	// 	// {
	// 	// 	auto AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	// 	// 	
	// 	// 	// WeaponRightHandMeshComponent->AttachToComponent(SkeletalMeshComponent, AttachmentTransformRules, "hand_r_sword_socket"); // hand_r_sword_socket
	// 	// 	// WeaponRightHandMeshComponent->AddLocalRotation(FRotator(0, 90, 90));
	// 	// 	// WeaponRightHandMeshComponent->SetVisibility(false); // "hand_r_sword_socket" NAME_None
	// 	//
	// 	// 	// auto AttachmentTransformRulesTwo = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	// 	// 	// WeaponBeltLeftMeshComponent->AttachToComponent(SkeletalMeshComponent, AttachmentTransformRules, "weapon_belt_left"); // weapon_belt_left
	// 	// 	// WeaponBeltLeftMeshComponent->AddLocalRotation(FRotator(0, 90, 90));
	// 	// }
	// 	
	// 	AnimInstance = Cast<UGHAnimInstance>(SkeletalMeshComponent->GetAnimInstance());
	// }

	if (IsPlayer)
	{
		auto PlayerController = UGameplayStatics::GetPlayerController(World, 0);
		if (PlayerController)
		{
			PlayerController->Possess(this);
		}
		
		SetWalkMode(EWalkMode::Run);
		// SetWalkMode(EWalkMode::Walk);
	}
	else
	{
		// not work in constructor
		AIControllerClass = AGHAIController::StaticClass();
		// this func possess also
		SpawnDefaultController();
		
		SetWalkMode(EWalkMode::Walk);
	}

	if (!IsPlayer)
	{
		auto CurrentDailyRoutine = GetCurrentDailyRoutine();
		if (CurrentDailyRoutine)
		{
			if (CurrentDailyRoutine->WaypointName.Len() > 0)
			{
				auto Waypoint = Helpers::GetWaypointByName(CurrentDailyRoutine->WaypointName);
				if (Waypoint)
				{
					SetActorLocation(Waypoint->Position);
					SetActorRotation(Waypoint->Direction);
				}
			}
		}
	}
	
	// work but SetWalkMode(EWalkMode::Run); error
	DelayedBeginPlay();
	
	// FTimerHandle DelayedBeginPlayTimer;
	// GetWorldTimerManager().SetTimer(DelayedBeginPlayTimer, this, &AGHNPC::DelayedBeginPlay, 0.001f, false);



	
	// EquipWeaponSword();

	// WithdrawingSword();

	// AnimInstance->PlayMontageByName("Attack");
}

void AGHNPC::DelayedBeginPlay()
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	// SetWalkMode(EWalkMode::Run);
	
	// Init from Table
	if (_table.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGHNPC::DelayedBeginPlay() _table.Type != ELuaValueType::Table"));
		return;
	}

	// auto Name = _table.GetField("name");
	// if (Name.Type == ELuaValueType::String)
	// 	ChangeTooltip(Name.ToString());

	auto Items = _table.GetField("items");
	if (Items.Type == ELuaValueType::Table)
	{
		auto TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Items);
		for (auto LuaValueItem : TableValues)
		{
			auto LuaValueItemName = LuaValueItem.GetField("name");
			auto LuaValueItemEquipped = LuaValueItem.GetField("equipped");
			if (LuaValueItemEquipped.Type == ELuaValueType::Bool && LuaValueItemEquipped.ToBool())
			{
				auto LuaValueModel = LuaValueItem.GetField("model");
				if (LuaValueModel.Type == ELuaValueType::String)
				{
					if (IsValid(WeaponLeftBeltMeshComponent))
					{
						// GameMode->GLTFRuntime->LoadModel(WeaponLeftBeltMeshComponent, LuaValueModel.ToString(), false);
						// WeaponLeftBeltMeshComponent->SetVisibility(true);
						// CurrentWeaponLeftBelt = LuaValueItem;

						break;
					}
					
					// if (IsValid(WeaponRightHandMeshComponent))
					// {
					// 	GameMode->GLTFRuntime->LoadModel(WeaponRightHandMeshComponent, LuaValueModel.ToString(), false);
					// }
				}
			}
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LuaValueItemName.ToString());
		}
	}
	
	FLuaValue DialogsLuaValue = _table.GetField("dialogs");
	if (DialogsLuaValue.Type == ELuaValueType::Table)
	{
		TArray<FLuaValue> TableKeys = ULuaBlueprintFunctionLibrary::LuaTableGetKeys(DialogsLuaValue);
		for (FLuaValue TableKey: TableKeys)
		{
			FLuaValue DialogLuaValue = DialogsLuaValue.GetField(TableKey.ToString());
			if (DialogLuaValue.Type != ELuaValueType::Table)
				continue;
			
			FDialog Dialog;
			Dialog.Name = TableKey.ToString();
			
			FLuaValue Nr = DialogLuaValue.GetField("nr");
			if (Nr.Type == ELuaValueType::Integer || Nr.Type == ELuaValueType::Number)
				Dialog.Nr = Nr.ToFloat();

			FLuaValue Important = DialogLuaValue.GetField("important");
			if (Important.Type == ELuaValueType::Bool)
				Dialog.Important = Important.ToBool();

			FLuaValue Condition = DialogLuaValue.GetField("condition");
			if (Condition.Type == ELuaValueType::Function || Condition.Type == ELuaValueType::Bool)
				Dialog.Condition = Condition;

			FLuaValue Information = DialogLuaValue.GetField("information");
			if (Information.Type == ELuaValueType::Function)
				Dialog.Information = Information;

			FLuaValue Description = DialogLuaValue.GetField("description");
			if (Description.Type == ELuaValueType::String)
				Dialog.Description = Description.ToString();
			
			FLuaValue Trade = DialogLuaValue.GetField("trade");
			if (Trade.Type == ELuaValueType::Bool)
				Dialog.Trade = Trade.ToBool();

			FLuaValue Permanent = DialogLuaValue.GetField("permanent");
			if (Permanent.Type == ELuaValueType::Bool)
				Dialog.Permanent = Permanent.ToBool();
			
			Dialogs.Add(Dialog);
		}

		Dialogs.Sort([](const FDialog& D1, const FDialog& D2) {return D1.Nr < D2.Nr;});
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AGHNPC::DelayedBeginPlay Items.Type != ELuaValueType::Table"));
	}
	
	// UseItemFromIndex(0);
}

// Called when this Pawn is possessed. Only called on the server (or in standalone).
void AGHNPC::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	auto PlayerController = Cast<AGHPlayerController>(NewController);
	auto AIController = Cast<AGHAIController>(NewController);
	if (PlayerController && !AIController)
	{
		// IsPlayer is not for controller variable
		// IsPlayer = true;

		ControlledBy = EControlledBy::Player;
		
		if (TooltipComponent)
			TooltipComponent->SetVisibility(false);
		
		// on top
		PlayerController->PlayerCameraManager->ViewPitchMin = -75.0f;
		
		// on down
		PlayerController->PlayerCameraManager->ViewPitchMax = 45.0f;

		// 	AnimInstance->RunStateMachine();
	}
	else if (AIController && !PlayerController)
	{
		ControlledBy = EControlledBy::AI;
		
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHNPC::PossessedBy AGHAIController");
	}
	else
	{
		ControlledBy = EControlledBy::Unknown;
		
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHNPC::PossessedBy UNKNOWN CONTROLLER!!!!");
	}
}

// Called when our Controller no longer possesses us. Only called on the server (or in standalone).
void AGHNPC::UnPossessed()
{
	Super::UnPossessed();

	ControlledBy = EControlledBy::None;
	
	// auto PlayerController = Cast<AGHPlayerController>(NewController);
	//
	// if (PlayerController)
	// {
	// 	// on top
	// 	PlayerController->PlayerCameraManager->ViewPitchMin = -75.0f;
	// 	// on down
	// 	PlayerController->PlayerCameraManager->ViewPitchMax = 45.0f;
	// }
	
	if (TooltipComponent)
		TooltipComponent->SetVisibility(true);
}

void AGHNPC::SetupStaticMeshComponents()
{
	auto SkeletalMeshComponent = GetMesh();
	if (!SkeletalMeshComponent)
		return;
	
	auto SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
	if (!SkeletalMesh)
		return;
	
	auto Skeleton = SkeletalMesh->GetSkeleton();
	if (!Skeleton)
		return;

	// UE_LOG(LogTemp, Warning, TEXT("shared skeleton pointer: %p"), Skeleton);
	

	// auto TempStaticMeshComponent = this->FindComponentByClass<UStaticMeshComponent>();
	// if (TempStaticMeshComponent && TempStaticMeshComponent->GetName().Contains("TempStaticMesh"))
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "SWORD FOUND!!!!!");
	// }
	// else
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "!TempStaticMeshComponent");
	// 	return;
	// }
	//
	// auto TempStaticMesh = TempStaticMeshComponent->GetStaticMesh();
	// if (!TempStaticMesh)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "!TempStaticMesh");
	// 	return;
	// }

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	// TODO: rebuild map on new skeleton (or visual)
	for (auto Socket : Skeleton->Sockets)
	{
		auto MorphedSkeletalMeshComponent = NewObject<UGHMorphedSkeletalMeshComponent>(this);
		MorphedSkeletalMeshComponent->RegisterComponent();
		// this->AddInstanceComponent(StaticMeshComponent);
		MorphedSkeletalMeshComponent->AttachToComponent(SkeletalMeshComponent,
			FAttachmentTransformRules::KeepRelativeTransform, Socket->SocketName);

		MorphedMeshComponentMap.Add(Socket->SocketName.ToString(), MorphedSkeletalMeshComponent);


		auto StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
		StaticMeshComponent->RegisterComponent();
		// this->AddInstanceComponent(StaticMeshComponent);
		StaticMeshComponent->AttachToComponent(SkeletalMeshComponent,
			FAttachmentTransformRules::KeepRelativeTransform, Socket->SocketName);

		StaticMeshComponentMap.Add(Socket->SocketName.ToString(), StaticMeshComponent);

		// GameMode->GLTFRuntime->LoadModelYF(StaticMeshComponent, "KM_ITMW_2H_SWORD_01", false);
	}
}

void AGHNPC::SetVisual(FString NewVisual)
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return;
	}

	// if (!GameInstance->AssetManager->AllAnimationGroupDataMapMap.Contains(NewVisual))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("!GameInstance->AssetManager->AllAnimationGroupDataMapMap.Contains(NewVisual)"));
	// 	return;
	// }
	
	// if (!GameInstance->SharedAnimationGroupDataMap.Contains(NewVisual))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("!GameInstance->SharedAnimationGroupDataMap.Contains(NewVisual)"));
	// 	return;
	// }

	// auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	// if (!GameMode)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("!GameMode"));
	// 	return;
	// }
	
	// auto AllAnimationGroupDataMap = GameInstance->AssetManager->AllAnimationGroupDataMapMap.Find(NewVisual);
	// if (!AllAnimationGroupDataMap)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("!AllAnimationGroupDataMap"));
	// 	return;
	// }
	//
	// if (!SharedAnimationGroupData->SkeletalMesh)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("!SharedAnimationGroupData->SkeletalMesh"));
	// 	return;
	// }

	auto SkeletalMeshVisual = GameInstance->AssetManager->GetVisual(NewVisual);
	if (!SkeletalMeshVisual)
	{
		UE_LOG(LogTemp, Warning, TEXT("!SkeletalMesh"));
		return;
	}
	
	auto SkeletalMeshComponent = GetMesh();
	if (!SkeletalMeshComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("!SkeletalMeshComponent"));
		return;
	}
	
	SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshVisual);
	Visual = NewVisual;

	// auto SkeletalMeshComponent = GetMesh();
	if (!SkeletalMeshComponent)
		return;
	
	auto SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
	if (!SkeletalMesh)
		return;
	
	auto Skeleton = SkeletalMesh->GetSkeleton();
	if (!Skeleton)
		return;

	// UE_LOG(LogTemp, Warning, TEXT("%s skeleton pointer: %p"), *this->GetName(), Skeleton);
	
	// if (SkeletalMeshComponent->DoesSocketExist(FName("SWORD")))
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Socket SWORD Exist!!!!!");
	// }
	// else
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Socket SWORD NOTTTTT Exist!!!!!");
	// }

	SetupStaticMeshComponents();

	// crush if not update
	AnimInstance = Cast<UGHAnimInstance>(SkeletalMeshComponent->GetAnimInstance());
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!AnimInstance"));
		return;
	}

	AnimInstance->UpdateAnimationPreset();

	AnimInstance->SetupBlendSpaces();

	// work only like this
	AnimInstance->RunStateMachine();
}
//
// void AGHNPC::SwapTextureInSkeletalMesh(USkeletalMesh* SkeletalMesh, int32 TextureVariant)
// {
// 	if (!SkeletalMesh)
// 		return;
//
// 	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
// 	if (!GameInstance)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
// 		return;
// 	}
//
// 	int32 TotalTextureIndex = 0;
// 	auto MaterialArray = SkeletalMesh->GetMaterials();
// 	for (int32 MaterialIndex = 0; MaterialIndex < MaterialArray.Num(); MaterialIndex++)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("MaterialSlotName: %s, MaterialIndex: %d"),
// 			*MaterialArray[MaterialIndex].MaterialSlotName.ToString(), MaterialIndex);
// 		
// 		auto MaterialInterface = MaterialArray[MaterialIndex].MaterialInterface;
// 		if (!MaterialInterface)
// 			continue;
//
// 		TArray<UTexture*> TextureArray;
// 		MaterialInterface->GetUsedTextures(TextureArray, EMaterialQualityLevel::Epic,
// 			true, ERHIFeatureLevel::SM6, true);
// 		for (int32 TextureIndex = 0; TextureIndex < TextureArray.Num(); TextureIndex++)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("TextureName: %s"), *TextureArray[TextureIndex]->GetName());
//
// 			if (TextureArray[TextureIndex]->GetName().Contains("Texture2D"))
// 			{
// 				TotalTextureIndex++;
//
// 				if (TextureVariant == TotalTextureIndex)
// 				{
// 					auto TextureData = GameInstance->AssetManager->GetTextureData("Hum_Body_Naked_V3_C0",
// 						TextureVariant);
//
// 					if (TextureData)
// 					{
// 						MaterialInterface->OverrideTexture(TextureArray[TextureIndex], TextureData->Texture,
// 							ERHIFeatureLevel::SM6);
// 						return;
// 					}
// 				}
// 			}
// 		}
// 	}
// }

void AGHNPC::SetBodyMesh(const FString& BodyMesh, int32 TextureV)
{
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return;
	}
	
	auto SkeletalMeshData = GameInstance->AssetManager->GetSkeletalMeshV2(BodyMesh);
	if (!SkeletalMeshData || !SkeletalMeshData->IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("!SkeletalMesh"));
		return;
	}

	TArray<FString> SkeletalMeshKeysArray;
	SkeletalMeshData->SkeletalMeshMap.GenerateKeyArray(SkeletalMeshKeysArray);

	auto SkeletalMeshLen = SkeletalMeshKeysArray.Num();
	auto SkeletalMeshComponentLen = SkeletalMeshComponentMap.Num();

	// Use index to shared ref int!!!!!!!!!!!!!!
	
	int32 TextureIndex = 0;
	int32 i = 0;
	for (auto& SkeletalMeshComponentTuple : SkeletalMeshComponentMap)
	{
		if (i < SkeletalMeshLen)
		{
			auto SkeletalMesh = SkeletalMeshData->SkeletalMeshMap.FindRef(SkeletalMeshKeysArray[i]);
			if (SkeletalMesh)
			{
				// // swap materials
				// auto SkeletalMeshMaterialsArray = SkeletalMesh->GetMaterials();
				// for (auto SkeletalMeshMaterial : SkeletalMeshMaterialsArray)
				// {
				// 	UE_LOG(LogTemp, Warning, TEXT("SkeletalMeshMaterial.MaterialSlotName: %s, index: %d"),
				// 		*SkeletalMeshMaterial.MaterialSlotName.ToString(), TextureIndex);
				//
				// 	if (TextureIndex != 0)
				// 	{
				// 		TextureIndex++;
				// 		continue;
				// 	}
				// 	
				// 	auto MaterialInterface = SkeletalMeshMaterial.MaterialInterface;
				// 	if (!MaterialInterface)
				// 	{
				// 		TextureIndex++;
				// 		continue;
				// 	}
				//
				// 	TArray<UTexture*> UsedTextureArray;
				// 	MaterialInterface->GetUsedTextures(UsedTextureArray, EMaterialQualityLevel::Epic,
				// 		true, ERHIFeatureLevel::SM6, true);
				//
				// 	UTexture* OriginalTexture = nullptr;
				// 	for (auto UsedTexture : UsedTextureArray)
				// 	{
				// 		UE_LOG(LogTemp, Warning, TEXT("UsedTexture->GetName(): %s"), *UsedTexture->GetName());
				// 		if (UsedTexture->GetName().Contains("2D"))
				// 			OriginalTexture = UsedTexture;
				// 	}
				//
				// 	if (!OriginalTexture)
				// 	{
				// 		TextureIndex++;
				// 		continue;
				// 	}
				//
				// 	auto TextureData = GameInstance->AssetManager->GetTextureData("Hum_Body_Naked_V3_C0", 0);
				// 	// FString FilePath = TEXT("C:/__Gothic_Converted_Data/Gothic 2/_TEXTURES/NPCs/Body/Hum_Body_Naked_V3_C0.tga");
				// 	// auto NewTexture = FImageUtils::ImportFileAsTexture2D(TextureData-);
				// 	if (!TextureData)
				// 	{
				// 		UE_LOG(LogTemp, Warning, TEXT("!TextureData"));
				// 		TextureIndex++;
				// 		continue;
				// 	}
				//
				// 	MaterialInterface->OverrideTexture(OriginalTexture, TextureData->Texture, ERHIFeatureLevel::SM6);
				// 	
				// 	TextureIndex++;
				// }
				
				SkeletalMeshComponentTuple.Value->SetSkeletalMesh(SkeletalMesh, false);
				UE_LOG(LogTemp, Warning, TEXT("SetBodyMesh set: %s to %s"),
					*SkeletalMeshComponentTuple.Key, *SkeletalMeshKeysArray[i]);
			}
		}
		
		i++;
	}

	// if (SkeletalMeshComponentMap.Num() <= 0)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("SkeletalMeshComponentMap.Num() <= 0"));
	// 	return;
	// }
	//
	// auto SkeletalMeshComponent1 = SkeletalMeshComponentMap.FindRef("Mesh1");
	// if (!SkeletalMeshComponent1)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("!SkeletalMeshComponent"));
	// 	return;
	// }
	//
	// SkeletalMeshComponent1->SetSkeletalMesh(SkeletalMeshData->SkeletalMeshMap, false);
	//
	// UE_LOG(LogTemp, Warning, TEXT("SetBodyMesh OK"));

// 	SetupStaticMeshComponents();
//
// 	// crush if not update
// 	AnimInstance = Cast<UGHAnimInstance>(SkeletalMeshComponent->GetAnimInstance());
// 	if (!AnimInstance)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("!AnimInstance"));
// 		return;
// 	}
//
// 	AnimInstance->UpdateAnimationPreset();
//
// 	AnimInstance->SetupBlendSpaces();
//
// 	// work only like this
// 	AnimInstance->RunStateMachine();
}

FLuaValue AGHNPC::GetLuaValue(FString Field)
{
	if (_table.Type != ELuaValueType::Table)
		return FLuaValue();

	FLuaValue TypeField = _table.GetField("__type");
	if (TypeField.Type != ELuaValueType::String)
		return FLuaValue();

	if (TypeField.ToString() != "C_Npc")
		return FLuaValue();

	FLuaValue InstanceField = _table.GetField("__instance");
	if (InstanceField.Type != ELuaValueType::String)
		return FLuaValue();
	
	return _table.GetField(Field);
}

void AGHNPC::StartStateLoop(FString StateName, FLuaValue Start, FLuaValue Loop, FLuaValue End, FLuaValue Waypoint)
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	CurrentStateName = StateName;
	CurrentStateTime = 0.0f;

	if (Start.Type == ELuaValueType::Function)
	{
		auto LuaValueOldSelf = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, UGHLuaState::StaticClass(), "self");
		ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", FLuaValue(this));

		ULuaBlueprintFunctionLibrary::LuaGlobalCallValueMulti(World, UGHLuaState::StaticClass(), Start, {});
	
		ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", LuaValueOldSelf);
	}

	// Loop can be null
	if (Loop.Type == ELuaValueType::Function)
	{
		FTimerDelegate StateLoopTimerDelegate;
		StateLoopTimerDelegate.BindUFunction(this, FName("StateLoop"), Loop, End, Waypoint);
		World->GetTimerManager().SetTimer(StateLoopTimerHandle, StateLoopTimerDelegate, .5f, true);
		// World->GetTimerManager().SetTimer(StateLoopTimerHandle, this, &AGHNPC::StateLoop, 0.5f, true);
	}
	else
	{
		CurrentStateName = "";
		CurrentStateTime = 0.0f;

		if (End.Type == ELuaValueType::Function)
		{
			auto LuaValueOldSelf = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, UGHLuaState::StaticClass(), "self");
			ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", FLuaValue(this));

			ULuaBlueprintFunctionLibrary::LuaGlobalCallValueMulti(World, UGHLuaState::StaticClass(), End, {});
	
			ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", LuaValueOldSelf);
		}
	}
}

void AGHNPC::StateLoop(FLuaValue Loop, FLuaValue End, FLuaValue Waypoint)
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);

	CurrentStateTime += 0.5f;
	
	auto LuaValueOldSelf = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, UGHLuaState::StaticClass(), "self");
	ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", FLuaValue(this));

	auto LuaValueLoopResult = ULuaBlueprintFunctionLibrary::LuaGlobalCallValueMulti(World,
		UGHLuaState::StaticClass(), Loop, {});
	
	ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", LuaValueOldSelf);

	if (LuaValueLoopResult.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueLoopResult.Num() <= 0"));
		return;
	}

	if (!(LuaValueLoopResult[0].Type == ELuaValueType::Integer || LuaValueLoopResult[0].Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueLoopResult[0].Type != Integer|Number"));
		return;
	}

	const auto LoopResult = LuaValueLoopResult[0].ToInteger();
	if (LoopResult & static_cast<int32>(ELoopResult::Continue))
	{
		// all ok
	}
	else if (LoopResult & static_cast<int32>(ELoopResult::End))
	{
		World->GetTimerManager().ClearTimer(StateLoopTimerHandle);

		CurrentStateName = "";
		CurrentStateTime = 0.0f;

		if (End.Type != ELuaValueType::Function)
		{
			UE_LOG(LogTemp, Warning, TEXT("End.Type != ELuaValueType::Function"));
			return;
		}
		
		LuaValueOldSelf = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, UGHLuaState::StaticClass(), "self");
		ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", FLuaValue(this));

		ULuaBlueprintFunctionLibrary::LuaGlobalCallValueMulti(World, UGHLuaState::StaticClass(), End, {});
	
		ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "self", LuaValueOldSelf);
	}
}

void AGHNPC::SetWalkMode(EWalkMode NewWalkMode)
{
	if (const auto MovementComp = GetCharacterMovement())
	{
		// TODO: properly values
		if (NewWalkMode == EWalkMode::Run)
			MovementComp->MaxWalkSpeed = 360.0f;
		else if (NewWalkMode == EWalkMode::Walk)
			MovementComp->MaxWalkSpeed = 180.0f;
		else if (NewWalkMode == EWalkMode::Sneak)
			MovementComp->MaxWalkSpeed = 90.0f;
		else if (NewWalkMode == EWalkMode::RunWeapon)
			MovementComp->MaxWalkSpeed = 360.0f;
		else if (NewWalkMode == EWalkMode::WalkWeapon)
			MovementComp->MaxWalkSpeed = 180.0f;
		else if (NewWalkMode == EWalkMode::SneakWeapon)
			MovementComp->MaxWalkSpeed = 90.0f;
	}

	WalkMode = NewWalkMode;
	
	if (!Visual.IsEmpty())
		AnimInstance->UpdateAnimationPreset();
}

// ---
// AI and Anim
// ---

FString AGHNPC::GetMaterialNameUnderFoot()
{
	// TODO: check lower foot bone to find pos and texture
	// TODO: add alpha texture check, ECC_Visibility or mb it not need
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return "";

	// FHitResult will hold all data returned by our line collision query
	FHitResult HitResult;
 
	// We set up a line trace from our current location to a point 1000cm ahead of us
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= 200;
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bReturnFaceIndex = true;
	CollisionQueryParams.bTraceComplex = true;

	World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,
		ECollisionChannel::ECC_Visibility, CollisionQueryParams);
 
	// You can use DrawDebug helpers and the log to help visualize and debug your trace queries.
	// DrawDebugLine(World, TraceStart, TraceEnd, HitResult.bBlockingHit ? FColor::Blue : FColor::Red,
	// 	false, 5.0f, 0, 2.0f);
	// UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());

	// is on air?
	if (!HitResult.bBlockingHit)
		return "";

	auto StaticMeshComponent = Cast<UStaticMeshComponent>(HitResult.GetComponent());
	if (!StaticMeshComponent)
		return "";
	
	int32 SectionIndex;
	auto MaterialInterface = StaticMeshComponent->GetMaterialFromCollisionFaceIndex(HitResult.FaceIndex,
		SectionIndex);
	if (!MaterialInterface)
		return "";
	
	return MaterialInterface->GetName();
}

void AGHNPC::PlayStepSound()
{
	// TODO: check assign texture to sound in opengothic
	
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return;
	}

	// if (!GameMode)
	// 	return;
	
	auto MaterialNameUnderFoot = GetMaterialNameUnderFoot();

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Magenta,
	// 	MaterialNameUnderFoot + " Material");

	if (MaterialNameUnderFoot.IsEmpty())
		return;
	
	// metal, sand

	FString AudioName = "";
	
	if (MaterialNameUnderFoot.Contains("NATURE") ||
		MaterialNameUnderFoot.Contains("GRASS") ||
		MaterialNameUnderFoot.Contains("GROUND"))
	{
		AudioName = "STEP_EARTH";
	}
	else if (MaterialNameUnderFoot.Contains("BODENDIELEN") ||
			 MaterialNameUnderFoot.Contains("WOOD"))
	{
		AudioName = "STEP_WOOD";
	}
	else if (MaterialNameUnderFoot.Contains("WATER"))
	{
		AudioName = "STEP_WATER";
	}
	else if (MaterialNameUnderFoot.Contains("STONE") ||
			 MaterialNameUnderFoot.Contains("CAVE"))
	{
		AudioName = "STEP_STONE";
	}
	else
	{
		AudioName = "TRO_STEPBOOM";
	}
	
	if (IsPlayer)
	{
		if (Visual == "HUMANS")
		{
			GameInstance->AssetManager->PlayAudioByName2D(AudioName);
		}
		// another sound from golem...
	}
}

// ---
// AI and Anim
// ---

void AGHNPC::Attack()
{
	if (AnimInstance)
	{
		AnimInstance->PlayMontageByName("Attack");
	}
}

void AGHNPC::SwapWeaponMesh(bool bIsDraw)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AGHNPC::SwapWeaponMesh");
	
	// check weapon category in args
	
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(this));
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return;
	}
	
	if (WeaponSwapEquipSocketName.IsEmpty() || WeaponSwapHandSocketName.IsEmpty())
		return;

	// WeaponSwapSocketName and WeaponSwapHandSocketName set in anim start func
	
	auto EquipMorphedSkeletalMeshComponent = MorphedMeshComponentMap.FindRef(WeaponSwapEquipSocketName);
	auto EquipStaticMeshComponent = StaticMeshComponentMap.FindRef(WeaponSwapEquipSocketName);

	auto InHandsMorphedSkeletalMeshComponent = MorphedMeshComponentMap.FindRef(WeaponSwapHandSocketName);
	auto InHandsStaticMeshComponent = StaticMeshComponentMap.FindRef(WeaponSwapHandSocketName);

	if (bIsDraw)
	{
		if (EquipMorphedSkeletalMeshComponent && EquipMorphedSkeletalMeshComponent->IsVisible())
		{
			if (InHandsMorphedSkeletalMeshComponent)
			{
				auto EquipSkeletalMesh = EquipMorphedSkeletalMeshComponent->GetSkeletalMeshAsset();
				if (EquipSkeletalMesh)
				{
					InHandsMorphedSkeletalMeshComponent->SetSkeletalMesh(EquipSkeletalMesh);
					InHandsMorphedSkeletalMeshComponent->SetVisibility(true);
					EquipMorphedSkeletalMeshComponent->SetVisibility(false);
				}
			}
		}
		if (EquipStaticMeshComponent && EquipStaticMeshComponent->IsVisible())
		{
			if (InHandsStaticMeshComponent)
			{
				auto EquipStaticMesh = EquipStaticMeshComponent->GetStaticMesh();
				if (EquipStaticMesh)
				{
					InHandsStaticMeshComponent->SetStaticMesh(EquipStaticMesh);
					InHandsStaticMeshComponent->SetVisibility(true);
					EquipStaticMeshComponent->SetVisibility(false);
				}
			}
		}

		if (WeaponSwapEquipSocketName.Compare("SWORD", ESearchCase::IgnoreCase) == 0)
		{
			ItemInHands = EquippedMeleeWeapon;

			auto LuaValueItemVisual = ItemInHands.GetField("visual");

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SwapWeaponMesh, ItemInHands: " + LuaValueItemVisual.ToString());

			
			// EquippedMeleeWeapon = FLuaValue();
		}
		else if (WeaponSwapEquipSocketName.Compare("BOW", ESearchCase::IgnoreCase) == 0)
		{
			ItemInHands = EquippedRangeWeapon;

			auto LuaValueItemVisual = ItemInHands.GetField("visual");

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SwapWeaponMesh, ItemInHands: " + LuaValueItemVisual.ToString());

			
			// EquippedMeleeWeapon = FLuaValue();
		}

		// @TODO: setup sounds
		if (GameInstance->AssetManager)
			GameInstance->AssetManager->PlayAudioByName2D("DRAWSOUND_ME");
	}
	else
	{
		if (EquipMorphedSkeletalMeshComponent &&
			InHandsMorphedSkeletalMeshComponent && InHandsMorphedSkeletalMeshComponent->IsVisible())
		{
			auto InHandsSkeletalMesh = InHandsMorphedSkeletalMeshComponent->GetSkeletalMeshAsset();
			if (InHandsSkeletalMesh)
			{
				EquipMorphedSkeletalMeshComponent->SetSkeletalMesh(InHandsSkeletalMesh);
				EquipMorphedSkeletalMeshComponent->SetVisibility(true);
				InHandsMorphedSkeletalMeshComponent->SetVisibility(false);
			}
		}
		if (EquipStaticMeshComponent &&
			InHandsStaticMeshComponent && InHandsStaticMeshComponent->IsVisible())
		{
			auto InHandsStaticMesh = InHandsStaticMeshComponent->GetStaticMesh();
			if (InHandsStaticMesh)
			{
				EquipStaticMeshComponent->SetStaticMesh(InHandsStaticMesh);
				EquipStaticMeshComponent->SetVisibility(true);
				InHandsStaticMeshComponent->SetVisibility(false);
			}
		}

		// EquippedMeleeWeapon = ItemInHands;
		ItemInHands = FLuaValue();
		
		if (GameInstance->AssetManager)
			GameInstance->AssetManager->PlayAudioByName2D("UNDRAWSOUND_ME");
	}

	
	//
	// auto WeaponInHandsStaticMeshComp = MorphedSkeletalMeshComponentMap.FindRef("RIGHTHAND");
	// if (!WeaponInHandsStaticMeshComp)
	// {
	// 	WeaponInHandsStaticMeshComp = MorphedSkeletalMeshComponentMap.FindRef("LEFTHAND");
	// 	if (!WeaponInHandsStaticMeshComp)
	// 		return;
	// }
	//
	// if (!ToSwapWeaponEquipStaticMeshComp)
	// 	return;
	//
	// auto WeaponEquipStaticMesh = ToSwapWeaponEquipStaticMeshComp->GetStaticMesh();
	// if (!WeaponEquipStaticMesh)
	// 	return;
	//
	// if (bIsDraw)
	// {
	// 	ToSwapWeaponEquipStaticMeshComp->SetVisibility(false);
	// 	WeaponInHandsStaticMeshComp->SetStaticMesh(WeaponEquipStaticMesh);
	// 	WeaponInHandsStaticMeshComp->SetVisibility(true);
	//
	// 	WeaponInHands = EquippedMeleeWeapon;
	// 	EquippedMeleeWeapon = FLuaValue();
	//
	// 	if (GameMode && GameMode->AssetManager)
	// 		GameMode->AssetManager->PlayAudio2D("DRAWSOUND_ME");
	// }
	// else
	// {
	// 	WeaponInHandsStaticMeshComp->SetVisibility(false);
	// 	ToSwapWeaponEquipStaticMeshComp->SetStaticMesh(WeaponEquipStaticMesh);
	// 	ToSwapWeaponEquipStaticMeshComp->SetVisibility(true);
	//
	// 	EquippedMeleeWeapon = WeaponInHands;
	// 	WeaponInHands = FLuaValue();
	//
	// 	if (GameMode && GameMode->AssetManager)
	// 		GameMode->AssetManager->PlayAudio2D("UNDRAWSOUND_ME");
	// }
}

// this just start animations
void AGHNPC::ReadyMeleeWeapon()
{
	// // AnimInstance->PlayMontageByName("T_MOVE_2_1HMOVE", false);
	// AnimInstance->PlayMontageByName("T_MOVE_2_1HMOVE", false);
	// return;
	
	UE_LOG(LogTemp, Log, TEXT("ReadyMeleeWeapon"));
}

void AGHNPC::RemoveAnyWeapon()
{

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AGHNPC::RemoveAnyWeapon");
	// // AnimInstance->PlayMontageByName("T_GOTHIT", false);
	// AnimInstance->PlayMontageByName("C_BOW_3", false);
	// return;
	
	// LuaValueEquipedMeleeWeapon
	UE_LOG(LogTemp, Log, TEXT("RemoveWeapon"));

	// TODO: check fists

	if (!AnimInstance)
	{
		ActionQueue->RemoveWeaponMeleeActionEnd();
		ActionQueue->RemoveWeaponRangeActionEnd();
		return;
	}
	
	if (ItemInHands.Type != ELuaValueType::Table)
	{
		ActionQueue->RemoveWeaponMeleeActionEnd();
		ActionQueue->RemoveWeaponRangeActionEnd();
		return;
	}

	auto LuaValueWeaponCategory = ItemInHands.GetField("mainflag");
	if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer || LuaValueWeaponCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer|Number"));
		ActionQueue->RemoveWeaponMeleeActionEnd();
		ActionQueue->RemoveWeaponRangeActionEnd();
		return;
	}

	auto LuaValueWeaponSubcategory = ItemInHands.GetField("flags");
	if (!(LuaValueWeaponSubcategory.Type == ELuaValueType::Integer || LuaValueWeaponSubcategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponSubcategory.Type != ELuaValueType::Integer|Number"));
		ActionQueue->RemoveWeaponMeleeActionEnd();
		ActionQueue->RemoveWeaponRangeActionEnd();
		return;
	}

	TArray<FString> AnimationRemoveWeapon = {""};
	auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
	if (WeaponCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		auto WeaponSubcategory = LuaValueWeaponSubcategory.ToInteger();
		if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Sword))
		{
			WeaponSwapEquipSocketName = "SWORD";
			WeaponSwapHandSocketName = "RIGHTHAND";
			
			AnimationRemoveWeapon[0] = "T_1HMOVE_2_MOVE";
			// sound = sword
		}
		else if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Axe))
		{
			WeaponSwapEquipSocketName = "SWORD";
			WeaponSwapHandSocketName = "RIGHTHAND";
			
			AnimationRemoveWeapon[0] = "T_1HMOVE_2_MOVE";
			// sound = axe
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong EItemSubcategory"));
			ActionQueue->RemoveWeaponMeleeActionEnd();
			ActionQueue->RemoveWeaponRangeActionEnd();
			return;
		}

		// if (!WeaponEquipStaticMeshComp)
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("!WeaponEquipStaticMesh"));
		// 	return;
		// }
		//
		// WeaponSwapSocketName = WeaponEquipStaticMeshComp;
	}
	else if (WeaponCategory & static_cast<int32>(EItemCategory::RangeWeapon))
	{
		auto WeaponSubcategory = LuaValueWeaponSubcategory.ToInteger();
		if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Bow))
		{
			WeaponSwapEquipSocketName = "BOW";
			WeaponSwapHandSocketName = "LEFTHAND";
			
			AnimationRemoveWeapon[0] = "T_BOWMOVE_2_MOVE";
			// add special sound
		}
		else if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Crossbow))
		{
			// WeaponSwapEquipSocketName = "BOW";
			// WeaponSwapHandSocketName = "LEFTHAND";
			
			// AnimationRemoveWeapon[0] = "T_BOWMOVE_2_MOVE";
			// add special sound
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong EItemSubcategory"));
			ActionQueue->RemoveWeaponMeleeActionEnd();
			ActionQueue->RemoveWeaponRangeActionEnd();
			return;
		}
	}

	if (AnimationRemoveWeapon.Num() == 1)
	{
		// na hody
		if (!AnimationRemoveWeapon[0].IsEmpty())
			AnimInstance->PlayMontageByName(AnimationRemoveWeapon[0], false);
	}
	else if (AnimationRemoveWeapon.Num() > 1)
		AnimInstance->PlayMontageQueue(AnimationRemoveWeapon);
}

// ---
// Inventory
// ---

FLuaValue AGHNPC::InventoryCreateItems(FLuaValue LuaValueItemInstance, int32 Count)
{
	auto bIsInstance = Helpers::CheckInstance(LuaValueItemInstance);
	if (!bIsInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!bIsInstance"));
		return FLuaValue();
	}
	
	auto NeedToAdd = FMath::Clamp(Count, 0, 999);

	auto LuaValueItems = _table.GetField("items");
	if (LuaValueItems.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueItems.Type != ELuaValueType::Table"));
		return FLuaValue();
	}

	auto LuaValueItemSubcategory = LuaValueItemInstance.GetField("flags");
	if (!(LuaValueItemSubcategory.Type == ELuaValueType::Integer ||
		LuaValueItemSubcategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponSubcategory.Type != LuaValueWeaponSubcategory::Integer||Number"));
		return FLuaValue();
	}
	
	auto WeaponSubcategory = LuaValueItemSubcategory.ToInteger();
	auto bIsMulti = (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Multi)) ? true : false;

	auto LuaValueItemInstanceName = LuaValueItemInstance.GetField("__instance").ToString();
	auto LuaValueItemArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(LuaValueItems);
	if (bIsMulti)
	{
		auto bIsFound = false;
		for (auto LuaValueItem : LuaValueItemArray)
		{
			if (LuaValueItem.Type != ELuaValueType::Table)
				continue;
		
			auto InstanceFieldName = LuaValueItem.GetField("__instance").ToString();
			if (LuaValueItemInstanceName == InstanceFieldName)
			{
				auto LuaValueAmount = LuaValueItem.GetField("amount");
				if (!(LuaValueAmount.Type == ELuaValueType::Integer || LuaValueAmount.Type == ELuaValueType::Number))
				{
					UE_LOG(LogTemp, Warning, TEXT("LuaValueAmount.Type != LuaValueAmount::Integer||Number"));
					continue;
				}

				LuaValueItem.SetField("amount", FLuaValue(LuaValueAmount.ToInteger() + NeedToAdd));
				bIsFound = true;
				break;
			}
		}
		if (!bIsFound)
		{
			auto LuaValueNewItemInstance = Helpers::CreateLuaObject(LuaValueItemInstance);
			LuaValueNewItemInstance.SetField("amount", NeedToAdd);
			LuaValueItems.SetFieldByIndex(LuaValueItemArray.Num() + 1, LuaValueNewItemInstance);
			
			return LuaValueNewItemInstance;
		}
	}
	else
	{
		int32 Index = 0;
		for (int32 i = LuaValueItemArray.Num() + 1; i <= LuaValueItemArray.Num() + NeedToAdd; i++)
		{
			auto LuaValueNewItemInstance =  Helpers::CreateLuaObject(LuaValueItemInstance);
			LuaValueItems.SetFieldByIndex(i, LuaValueNewItemInstance);
			Index = i;
		}

		if (Index > 0)
		{
			LuaValueItems = _table.GetField("items");
			return LuaValueItems.GetFieldByIndex(Index);
		}
	}

	return FLuaValue();
}

// ---
// Inventory
// ---


// ---
// Input
// ---

void AGHNPC::InputMeleeWeaponToggle()
{
	// UE_LOG(LogTemp, Log, TEXT("InputMeleeWeaponToggle"));

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AGHNPC::InputMeleeWeaponToggle");
	// @TODO: check fists

	// we have something in hands
	if (ItemInHands.Type == ELuaValueType::Table)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "InputMeleeWeaponToggle 1");
		
		auto LuaValueWeaponCategory = ItemInHands.GetField("mainflag");
		if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer ||
			LuaValueWeaponCategory.Type == ELuaValueType::Number))
		{
			UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer|Number"));
			return;
		}
		
		auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
		if (WeaponCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
		{
			ActionQueue->RemoveWeaponMeleeActionAdd();
		}
		else if (WeaponCategory & static_cast<int32>(EItemCategory::RangeWeapon))
		{
			ActionQueue->RemoveWeaponMeleeActionAdd();
			ActionQueue->DrawWeaponMeleeActionAdd();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "InputMeleeWeaponToggle 2");
		// if (EquippedMeleeWeapon.Type == ELuaValueType::Table)
		// {}
		ActionQueue->DrawWeaponMeleeActionAdd();
	}
}

void AGHNPC::InputRangeWeaponToggle()
{
	// UE_LOG(LogTemp, Log, TEXT("InputMeleeWeaponToggle"));

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AGHNPC::InputRangeWeaponToggle");
	// @TODO: check fists

	// we have something in hands
	if (ItemInHands.Type == ELuaValueType::Table)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "InputRangeWeaponToggle 1");
		
		auto LuaValueWeaponCategory = ItemInHands.GetField("mainflag");
		if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer ||
			LuaValueWeaponCategory.Type == ELuaValueType::Number))
		{
			UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer|Number"));
			return;
		}
		
		auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
		if (WeaponCategory & static_cast<int32>(EItemCategory::RangeWeapon))
		{
			ActionQueue->RemoveWeaponRangeActionAdd();
		}
		else if (WeaponCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
		{
			ActionQueue->RemoveWeaponMeleeActionAdd();
			ActionQueue->DrawWeaponRangeActionAdd();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "InputRangeWeaponToggle 2");
		// if (EquippedMeleeWeapon.Type == ELuaValueType::Table)
		// {}
		ActionQueue->DrawWeaponRangeActionAdd();
	}
}

void AGHNPC::InputMagicWeaponToggle()
{
	AnimInstance->PlayMontageByName("S_RUN", true, false, false);
}

// ---
// Input
// ---

void AGHNPC::RemoveWeaponLeftBelt()
{
	UE_LOG(LogTemp, Log, TEXT("RemoveWeaponLeftBelt"));
	
	if (EquippedMeleeWeapon.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponInHands.Type != ELuaValueType::Table"));
		return;
	}

	if (EquippedMeleeWeapon.Type == ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponLeftBelt.Type == ELuaValueType::Table"));
		return;
	}
	
	if (AnimInstance)
	{
		AnimInstance->PlayMontageByName("SheathingSword");
	}
}

void AGHNPC::EquipMesh(FLuaValue LuaValueWeapon)
{
	UE_LOG(LogTemp, Log, TEXT("NPC EquipWeapon"));
	
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	// if (!GameMode || !GameMode->AssetManager)
	// 	return;

	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance || !GameInstance->AssetManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance || !GameInstance->AssetManager"));
		return;
	}

	auto LuaValueItemVisual = LuaValueWeapon.GetField("visual");
	if (LuaValueItemVisual.Type != ELuaValueType::String)
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponModel.Type != ELuaValueType::String"));
		return;
	}

	auto ItemVisual = LuaValueItemVisual.ToString();
	if (ItemVisual.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemVisual.IsEmpty()"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("EquipMesh, ItemVisual: %s"), *ItemVisual);

	auto LuaValueWeaponCategory = LuaValueWeapon.GetField("mainflag");
	if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer ||
		LuaValueWeaponCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer|Number"));
		return;
	}

	auto LuaValueWeaponSubcategory = LuaValueWeapon.GetField("flags");
	if (!(LuaValueWeaponSubcategory.Type == ELuaValueType::Integer ||
		LuaValueWeaponSubcategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponSubcategory.Type != ELuaValueType::Integer|Number"));
		return;
	}

	auto ItemCategory = LuaValueWeaponCategory.ToInteger();
	auto ItemSubcategory = LuaValueWeaponSubcategory.ToInteger();

	UStaticMeshComponent* EquipStaticMesh = nullptr;
	UGHMorphedSkeletalMeshComponent* EquipMorphedMesh = nullptr;
	
	FString SocketName;
	if (ItemCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		if (ItemSubcategory & static_cast<int32>(EItemSubcategory::Axe) ||
			ItemSubcategory & static_cast<int32>(EItemSubcategory::Sword))
		{
			EquipStaticMesh = StaticMeshComponentMap.FindRef("SWORD");
			EquipMorphedMesh = MorphedMeshComponentMap.FindRef("SWORD");

			if (EquipStaticMesh == nullptr && EquipMorphedMesh == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Wrong sockets"));
				return;
			}

			auto StaticMeshData = GameInstance->AssetManager->GetStaticMeshV2(ItemVisual);
			if (StaticMeshData && StaticMeshData->IsValid())
			{
				if (EquipStaticMesh)
				{
					EquipStaticMesh->SetStaticMesh(StaticMeshData->StaticMesh);
					EquipStaticMesh->SetVisibility(true);
				}
				if (EquipMorphedMesh)
				{
					EquipMorphedMesh->SetVisibility(false);
				}
			}
			else
			{
				auto MorphedMeshData = GameInstance->AssetManager->GetMorphedMeshV2(ItemVisual);
				if (MorphedMeshData && MorphedMeshData->IsValid())
				{
					if (EquipMorphedMesh)
					{
						EquipMorphedMesh->SetMorphData(*MorphedMeshData);
						// EquipMorphedMesh->SetSkeletalMesh(MorphedMeshData->SkeletalMesh);
						// EquipMorphedMesh->MorphData = *MorphedMeshData;
						
						EquipMorphedMesh->SetVisibility(true);
					}
					if (EquipStaticMesh)
					{
						EquipStaticMesh->SetVisibility(false);
					}
				}
			}
			
			// if (EquipStaticMesh)
			// {
			// 	auto StaticMeshData = GameMode->AssetManager->GetStaticMeshV2(ItemVisual);
			// }
			//
			// if (EquipMorphedSkeletalMesh)
			// 	EquipMorphedSkeletalMesh->SetVisibility(false);

			// if (EquipStaticMesh)
			// {
			// 	// auto StaticMeshData = GameMode->AssetManager->GetStaticMesh(ItemVisual);
			// 	auto StaticMeshData = GameMode->AssetManager->GetStaticMeshV2(ItemVisual);
			// 	if (StaticMeshData && StaticMeshData->IsValid())
			// 	{
			// 		EquipStaticMesh->SetStaticMesh(StaticMeshData->StaticMesh);
			// 		EquipStaticMesh->SetVisibility(true);
			// 	}
			// 	else
			// 	{
			// 		auto SkeletonMeshData = GameMode->AssetManager->GetStaticMeshV2(ItemVisual);
			// 		if (StaticMeshData && StaticMeshData->IsValid())
			// 		{
			// 			EquipStaticMesh->SetStaticMesh(StaticMeshData->StaticMesh);
			// 			EquipStaticMesh->SetVisibility(true);
			// 		}
			// 	}
			// }
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong EItemSubcategory"));
			return;
		}
		
		// GameMode->GLTFRuntime->LoadModelYF(WeaponEquipStaticMesh, LuaValueWeaponVisual.ToString(), false);
		
		// CurrentWeaponLeftBelt = LuaValueWeapon;

		EquippedMeleeWeapon = LuaValueWeapon;
		//LuaValueItemVisual

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LuaValueItemVisual: " + LuaValueItemVisual.ToString());

		auto LuaValueKeyArray = ULuaBlueprintFunctionLibrary::LuaTableGetKeys(EquippedMeleeWeapon);
		auto LuaValueValueArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(EquippedMeleeWeapon);
		if (LuaValueKeyArray.Num() == LuaValueValueArray.Num())
		{
			for (int32 i = 0; i < LuaValueKeyArray.Num(); i++)
			{
				UE_LOG(LogTemp, Warning, TEXT("item, key: %s,     value: %s"),
					*LuaValueKeyArray[i].ToString(), *LuaValueValueArray[i].ToString());
			}
		}
	}
	else if (ItemCategory & static_cast<int32>(EItemCategory::RangeWeapon))
	{
		if (ItemSubcategory & static_cast<int32>(EItemSubcategory::Bow))
		{
			EquipStaticMesh = StaticMeshComponentMap.FindRef("BOW");
			EquipMorphedMesh = MorphedMeshComponentMap.FindRef("BOW");
			
			auto MorphedMeshData = GameInstance->AssetManager->GetMorphedMeshV2(ItemVisual);
			if (MorphedMeshData && MorphedMeshData->IsValid())
			{
				if (EquipMorphedMesh)
				{
					EquipMorphedMesh->SetMorphData(*MorphedMeshData);
					// EquipMorphedMesh->SetSkeletalMesh(MorphedMeshData->SkeletalMesh);
					// EquipMorphedMesh->MorphData = *MorphedMeshData;
					
					EquipMorphedMesh->SetVisibility(true);
				}
				if (EquipStaticMesh)
				{
					EquipStaticMesh->SetVisibility(false);
				}
			}
			else
			{
				auto StaticMeshData = GameInstance->AssetManager->GetStaticMeshV2(ItemVisual);
				if (StaticMeshData && StaticMeshData->IsValid())
				{
					if (EquipStaticMesh)
					{
						EquipStaticMesh->SetStaticMesh(StaticMeshData->StaticMesh);
						EquipStaticMesh->SetVisibility(true);
					}
					if (EquipMorphedMesh)
					{
						EquipMorphedMesh->SetVisibility(false);
					}
				}
			}
			
			// if (EquipStaticMesh)
			// 	EquipStaticMesh->SetVisibility(false);
			//
			// if (EquipMorphedMesh)
			// {
			// 	auto MorphData = GameMode->AssetManager->GetMorphedSkeletalMesh(ItemVisual);
			// 	if (MorphData.IsValid())
			// 	{
			// 		EquipMorphedMesh->SetSkeletalMesh(MorphData.SkeletalMesh);
			// 		EquipMorphedMesh->MorphData = MorphData;
			//
			// 		// EquipMorphedSkeletalMesh->PlayAnimationByName("S_READY");
			// 		// EquipMorphedSkeletalMesh->PlayAnimationByName("S_SHOOT");
			//
			// 		EquipMorphedMesh->SetVisibility(true);
			// 	}
			// }
		}
		else if (ItemSubcategory & static_cast<int32>(EItemSubcategory::Crossbow))
		{
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong EItemSubcategory"));
			return;
		}

		EquippedRangeWeapon = LuaValueWeapon;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong EItemCategory"));
		return;
	}
}

void AGHNPC::SetHeadMesh(const FString& HeadName)
{
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance || !GameInstance->AssetManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance || !GameInstance->AssetManager"));
		return;
	}
	
	auto HeadMorphedMesh = MorphedMeshComponentMap.FindRef("HEAD");
	if (!HeadMorphedMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("!HeadMorphedMesh"));
		return;
	}
	
	auto MorphedMeshData = GameInstance->AssetManager->GetMorphedMeshV2(HeadName);
	if (MorphedMeshData && MorphedMeshData->IsValid())
	{
		HeadMorphedMesh->SetMorphData(*MorphedMeshData);
		// EquipMorphedMesh->SetSkeletalMesh(MorphedMeshData->SkeletalMesh);
		// EquipMorphedMesh->MorphData = *MorphedMeshData;
	}
}

void AGHNPC::UnequipWeapon(FLuaValue LuaValueWeapon)
{
	UE_LOG(LogTemp, Log, TEXT("UnequipWeapon"));
	
	const auto LuaValueWeaponCategory = LuaValueWeapon.GetField("mainflag");
	if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer || LuaValueWeaponCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer|Number"));
		return;
	}

	const auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
	if (WeaponCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		if (IsValid(WeaponLeftBeltMeshComponent))
		{
			WeaponLeftBeltMeshComponent->SetVisibility(false);
			// CurrentWeaponLeftBelt = FLuaValue();
		}
	}

	CurrentFightMode = EFightMode::None;
}

void AGHNPC::UseItemFromIndex(const int32 Index)
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	auto LuaValueItems = _table.GetField("items");
	if (LuaValueItems.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueItems.Type != ELuaValueType::Table"));
		return;
	}

	auto LuaValueItemsValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(LuaValueItems);
	if (Index >= LuaValueItemsValues.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Index >= LuaValueItemsValues.Num()"));
		return;
	}

	auto LuaValueItem = LuaValueItemsValues[Index];
	if (LuaValueItem.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueItem.Type != ELuaValueType::Table"));
		return;
	}
	auto LuaValueItemCategory = LuaValueItem.GetField("mainflag");
	if (!(LuaValueItemCategory.Type == ELuaValueType::Integer || LuaValueItemCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueItemCategory.Type != ELuaValueType::Integer|Number"));
		return;
	}
	auto ItemCategory = LuaValueItemCategory.ToInteger();
	if (ItemCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "CAST MeleeWeapon");

		auto LuaValueItemModel = LuaValueItem.GetField("model");
		if (LuaValueItemModel.Type != ELuaValueType::String)
		{
			UE_LOG(LogTemp, Warning, TEXT("LuaValueItemModel.Type != ELuaValueType::String"));
			return;
		}
		if (IsValid(WeaponLeftBeltMeshComponent))
		{
			// GameMode->GLTFRuntime->LoadModel(WeaponLeftBeltMeshComponent, LuaValueItemModel.ToString(), false);
			// WeaponLeftBeltMeshComponent->SetVisibility(true);
			AnimInstance->PlayMontageByName("WithdrawingSwordMontage");
		}
	}
	if (ItemCategory & static_cast<int32>(EItemCategory::Munition))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "CAST Munition");
	}
	if (ItemCategory & static_cast<int32>(EItemCategory::RangeWeapon))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "CAST RangeWeapon");
	}

	
	// auto TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Items);
	// for (auto TableValue : TableValues)
	// {
	// 	auto LuaValueItemName = TableValue.GetField("name");
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LuaValueItemName.ToString());
	// }

	// auto TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Items);
	// for (int32 i = 0; i < TableValues.Num(); i++)
	// {
	// 	FString JoinedStr = "item index: ";
	// 	JoinedStr += FString::FromInt(i) + " ";
	// 	JoinedStr += TableValues[i].GetField("name").ToString();
	//
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, JoinedStr);
	// }
}

void AGHNPC::ChangeTooltip(FString Text)
{
	if (Tooltip)
		Tooltip->UpdateText(Text);
}

void AGHNPC::PrintInventory()
{
	if (_table.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("_table.Type != ELuaValueType::Table"));
		return;
	}
	
	FLuaValue Items = _table.GetField("items");
	if (Items.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("Items.Type != ELuaValueType::Table"));
		return;
	}
	
	TArray<FLuaValue> TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Items);
	for (FLuaValue TableValue : TableValues)
	{
		FLuaValue LuaValueItemName = TableValue.GetField("name");
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LuaValueItemName.ToString());
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHNPC::PrintInventory");
}

void AGHNPC::PrintDialogs()
{
	if (_table.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("_table.Type != ELuaValueType::Table"));
		return;
	}
	
	FLuaValue Items = _table.GetField("dialogs");
	if (Items.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("Items.Type != ELuaValueType::Table"));
		return;
	}

	TArray<FLuaValue> TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetKeys(Items);
	for (FLuaValue TableValue : TableValues)
	{
		//FLuaValue LuaValueItemName = TableValue.GetField("name");
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TableValue.ToString());
	}
	// TArray<FLuaValue> TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Items);
	// for (FLuaValue TableValue : TableValues)
	// {
	// 	FLuaValue LuaValueItemName = TableValue.GetField("name");
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LuaValueItemName.ToString());
	// }

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHNPC::PrintDialogs");
}

UGHNPCDailyRoutine* AGHNPC::GetCurrentDailyRoutine()
{
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(this));
	
	auto tbigger = [&](int h1, int m1, int h2, int m2)
	{
		return h1 > h2 || (h1 == h2 && m1 > m2);
	};
	
	auto tsmaller = [&](int h1, int m1, int h2, int m2)
	{
		return h1 < h2 || (h1 == h2 && m1 < m2);
	};
	
	auto trange = [&](int h1, int m1, int h, int m, int h2, int m2)
	{
		return tbigger(h, m, h1, m1) && tsmaller(h, m, h2, m2);
	};
	
	for (auto& DailyRoutine: DailyRoutineArray)
	{
		// Game time 13:30
		// 1 Routine time 12:00 - 00:00
		// 2 Routine time 00:00 - 12:00
		// 3 Routine time 23:00 - 01:00
		
		bool crossesZero = DailyRoutine->StopHour < DailyRoutine->StartHour;
		bool InTime = false;
	
		if (!crossesZero)
		{
			InTime = trange(DailyRoutine->StartHour, DailyRoutine->StartMinute,
				GameMode->GameTimeHours, GameMode->GameTimeMinutes,
				DailyRoutine->StopHour, DailyRoutine->StopMinute);
		}
		else
		{
			InTime = trange(DailyRoutine->StartHour, DailyRoutine->StartMinute, GameMode->GameTimeHours, GameMode->GameTimeMinutes, 24, 0) &&
				   trange(0, 0, GameMode->GameTimeHours, GameMode->GameTimeMinutes, DailyRoutine->StopHour, DailyRoutine->StopMinute);
		}

		if (InTime)
			return DailyRoutine;
	}

	return nullptr;
}

// Called every frame
void AGHNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(this));

	// 340 for items
	if (IsPlayer)
	{
		// TArray<AGHNPC*> NearNpcArray = {}; // set def size 128, and add to class with flush
		TArray<TTuple<AGHNPC*, double>> NearNpcTupleArray = {}; // set def size 128, and add to class with flush
		// Focus
		for (auto NpcIter: GameMode->NpcArray)
		{
			if (this == NpcIter)
				continue;
			
			auto Dist = FVector::Dist(GetActorLocation(), NpcIter->GetActorLocation());
			if (Dist <= 500)
			{
				NearNpcTupleArray.Emplace(NpcIter, Dist);
			}

			NpcIter->ChangeTooltip("");
		}

		TTuple<AGHNPC*, double> BestFocusNpcTuple = {nullptr, 0.0};
		for (auto NearNpcIter : NearNpcTupleArray)
		{
			auto LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),
				NearNpcIter.Key->GetActorLocation());
			auto RelativeRotation = LookAtRotation - GetActorRotation();
			
			if (RelativeRotation.Pitch < -35 || RelativeRotation.Pitch > 35)
				continue;

			if (RelativeRotation.Yaw < -35 || RelativeRotation.Yaw > 35)
				continue;

			auto TotalAngle = abs(RelativeRotation.Yaw) + abs(RelativeRotation.Pitch);

			auto DistCoef = 1.2f;
			auto RotCoef = 1.0f;
			auto DistValue = 1.0 - (NearNpcIter.Value / 500) * DistCoef;
			auto RotValue = 1.0 - (TotalAngle / (35 * 2)) * RotCoef;
			auto TotalValue = DistValue + RotValue;
			
			auto Info = FString::Printf(TEXT("Dist: %f, Angle: %f, TotalValue: %f"),
				NearNpcIter.Value, TotalAngle, TotalValue);
			// NearNpcIter.Key->ChangeTooltip(Info);

			if (BestFocusNpcTuple.Value < TotalValue)
				BestFocusNpcTuple = {NearNpcIter.Key, TotalValue};
			
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			// 	"Rot: " + FString::SanitizeFloat(RelativeRotation.Yaw, 3));

			// InAngleNpcTupleArray.Emplace(NearNpcIter.Key, TotalAngle);
		}

		if (BestFocusNpcTuple.Key)
		{
			auto Name = BestFocusNpcTuple.Key->_table.GetField("name");
			if (Name.Type == ELuaValueType::String)
			{
				BestFocusNpcTuple.Key->ChangeTooltip(Name.ToString());
			}
		}
	}

	ActionQueue->ExecuteActionQueue();
}

// Called to bind functionality to input
void AGHNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGHNPC::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	auto SkeletalMeshComp = GetMesh();
	if (!SkeletalMeshComp)
		return;
	
	for (auto& SkeletalMeshComponentMapTuple : SkeletalMeshComponentMap)
	{
		if (!SkeletalMeshComponentMapTuple.Value)
			continue;
		
		SkeletalMeshComponentMapTuple.Value->SetLeaderPoseComponent(SkeletalMeshComp);
	}
}

void AGHNPC::Test_LoadSkeletalMesh()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Test_LoadSkeletalMesh");
	
	auto PathToModel = "C:/archolos/test/HUM_BODY_NAKED0.gltf";
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAssetSkeletonAndMesh = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToModel,false, LoaderConfig);
	if (!RuntimeAssetSkeletonAndMesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "RuntimeAssetSkeletonAndMesh ERR");
		return;
	}
	
	FglTFRuntimeSkeletalMeshConfig RuntimeSkeletalMeshConfig;
	// RuntimeSkeletalMeshConfig.Skeleton = SkeletonTest;
	// RuntimeSkeletalMeshConfig.bOverwriteRefSkeleton = true;
	auto LoadedSkeletalMesh = RuntimeAssetSkeletonAndMesh->LoadSkeletalMesh(0, 0, RuntimeSkeletalMeshConfig);
	if (!LoadedSkeletalMesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LoadedSkeletalMesh ERR");
		return;
	}

	auto SkeletalMeshComp = GetMesh();
	if (!SkeletalMeshComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SkeletalMeshComp ERR");
		return;
	}
	
	SkeletalMeshComp->SetSkeletalMesh(LoadedSkeletalMesh);
}

void AGHNPC::Test_LoadAnimation()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Test_LoadAnimation");
	
	auto PathToFileAnim = "C:/archolos/test/HUM_RUNLOOP_M01.gltf";
	FglTFRuntimeConfig LoaderConfig;
	LoaderConfig.TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
	auto RuntimeAssetAnimation = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToFileAnim,
		false, LoaderConfig);
	if (!RuntimeAssetAnimation)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "RuntimeAssetAnimation ERR");
		return;
	}

	auto SkeletalMeshComp = GetMesh();
	if (!SkeletalMeshComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SkeletalMeshComp ERR");
		return;
	}

	auto SkeletalMesh = SkeletalMeshComp->GetSkeletalMeshAsset();
	if (!SkeletalMesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SkeletalMesh ERR");
		return;
	}
	
	FglTFRuntimeSkeletalAnimationConfig RSAConfig;
	auto Anim = RuntimeAssetAnimation->LoadSkeletalAnimation(SkeletalMesh, 0, RSAConfig);
	if (!Anim)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Anim ERR");
		return;
	}
	
	FMontageBlendSettings BlendInSettings(0.25f);
	FMontageBlendSettings BlendOutSettings(0.25f);
	auto AnimMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage_WithBlendSettings(Anim,
		"DefaultSlot", BlendInSettings, BlendOutSettings, 1, 1, -1);
	if (!AnimMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimMontage ERR");
		return;
	}

	AnimInstance = Cast<UGHAnimInstance>(SkeletalMeshComp->GetAnimInstance());
	if (!AnimInstance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimInstance ERR");
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Montage_Play ->");
	auto MontageLenght = AnimInstance->Montage_Play(AnimMontage);
	auto MLen = FString::SanitizeFloat(MontageLenght);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "MontageLenght: " + MLen);
}

void AGHNPC::Test_PlayAnimation()
{
	auto SkeletalMeshComp = GetMesh();
	if (!SkeletalMeshComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SkeletalMeshComp ERR");
		return;
	}
	
	AnimInstance = Cast<UGHAnimInstance>(SkeletalMeshComp->GetAnimInstance());
	if (!AnimInstance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimInstance ERR");
		return;
	}

	auto AnimSequence = AnimInstance->AnimationTest;
	if (!AnimSequence)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimSequence ERR");
		return;
	}
	
	FMontageBlendSettings BlendInSettings(0.0f);
	FMontageBlendSettings BlendOutSettings(0.0f);
	auto AnimMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage_WithBlendSettings(AnimSequence,
		"DefaultSlot", BlendInSettings, BlendOutSettings, 1, 1, -1);
	if (!AnimMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "AnimMontage ERR");
		return;
	}
		
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Montage_Play ->");
	auto MontageLenght = AnimInstance->Montage_Play(AnimMontage);
	auto MLen = FString::SanitizeFloat(MontageLenght);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "MontageLenght: " + MLen);
}

