// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GHPlayerController.h"

#include "Game/GHAnimInstance.h"


AGHPlayerController::AGHPlayerController()
{
	// Input Action
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Look"));
	MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Move"));
	JumpAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Jump"));
	InventoryAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Inventory"));
	DialogAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Dialog"));
	StatsAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Stats"));
	QuestAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Quest"));
	PossesAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Posses"));
	
	RotIncAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_RotInc"));
	RotDecAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_RotDec"));
	ChangeAxisAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_ChangeAxis"));

	MoveForwardAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Move_Forward"));
	MoveBackwardAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Move_Backward"));
	MoveLeftAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Move_Left"));
	MoveRightAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Move_Right"));
	TurnLeftAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Turn_Left"));
	TurnRightAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_Turn_Right"));

	// Item slot
	MeleeWeaponAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_MeleeWeapon"));
	RangeWeaponAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_RangeWeapon"));
	MagicWeaponAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Player/Input/Actions/IA_MagicWeapon"));
	
	// Input Mapping Context
	GothicInputMappingContext = LoadObject<UInputMappingContext>(nullptr,TEXT("/Game/Player/Input/GothicInputMappingContext"));
}

void AGHPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AGHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (GothicInputMappingContext)
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(GothicInputMappingContext, 0);
		}
	}

	if (InputComponent)
	{
		if (auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// Looking
			if (LookAction)
				EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

			// Moving
			if (MoveAction)
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

			// Jumping
			if (JumpAction)
			{
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJumping);
			}

			// Open inventory
			if (InventoryAction)
				EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ThisClass::Inventory);

			// Start dialog
			if (DialogAction)
				EnhancedInputComponent->BindAction(DialogAction, ETriggerEvent::Started, this, &ThisClass::Dialog);
			
			// Open stats
			if (StatsAction)
				EnhancedInputComponent->BindAction(StatsAction, ETriggerEvent::Started, this, &ThisClass::Stats);

			// Open quests
			if (QuestAction)
				EnhancedInputComponent->BindAction(QuestAction, ETriggerEvent::Started, this, &ThisClass::Quest);

			// Posses on npc
			if (PossesAction)
				EnhancedInputComponent->BindAction(PossesAction, ETriggerEvent::Started, this, &ThisClass::Posses);

			// rot
			// Posses on npc
			if (RotIncAction)
				EnhancedInputComponent->BindAction(RotIncAction, ETriggerEvent::Started, this, &ThisClass::RotInc);

			// Posses on npc
			if (RotDecAction)
				EnhancedInputComponent->BindAction(RotDecAction, ETriggerEvent::Started, this, &ThisClass::RotDec);

			// Posses on npc
			if (ChangeAxisAction)
				EnhancedInputComponent->BindAction(ChangeAxisAction, ETriggerEvent::Started, this, &ThisClass::ChangeAxisCF);

			// Move Forward
			if (MoveForwardAction)
			{
				EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Started, this, &ThisClass::StartMoveForward);
				EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ThisClass::StopMoveForward);
			}

			// Move backward
			if (MoveBackwardAction)
			{
				EnhancedInputComponent->BindAction(MoveBackwardAction, ETriggerEvent::Started, this, &ThisClass::StartMoveBackward);
				EnhancedInputComponent->BindAction(MoveBackwardAction, ETriggerEvent::Completed, this, &ThisClass::StopMoveBackward);
			}

			// Move left
			if (MoveLeftAction)
			{
				EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Started, this, &ThisClass::StartMoveLeft);
				EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Completed, this, &ThisClass::StopMoveLeft);
			}

			// Move right
			if (MoveRightAction)
			{
				EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Started, this, &ThisClass::StartMoveRight);
				EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &ThisClass::StopMoveRight);
			}

			// Turn left
			if (TurnLeftAction)
			{
				EnhancedInputComponent->BindAction(TurnLeftAction, ETriggerEvent::Started, this, &ThisClass::StartTurnLeft);
				EnhancedInputComponent->BindAction(TurnLeftAction, ETriggerEvent::Completed, this, &ThisClass::StopTurnLeft);
			}

			// Move right
			if (TurnRightAction)
			{
				EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Started, this, &ThisClass::StartTurnRight);
				EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Completed, this, &ThisClass::StopTurnRight);
			}

			// Ready/remove melee weapon
			if (MeleeWeaponAction)
				EnhancedInputComponent->BindAction(MeleeWeaponAction, ETriggerEvent::Started, this, &ThisClass::MeleeWeaponToggle);

			// Ready/remove range weapon
			if (RangeWeaponAction)
				EnhancedInputComponent->BindAction(RangeWeaponAction, ETriggerEvent::Started, this, &ThisClass::RangeWeaponToggle);

			// Ready/remove magic weapon
			if (MagicWeaponAction)
				EnhancedInputComponent->BindAction(MagicWeaponAction, ETriggerEvent::Started, this, &ThisClass::MagicWeaponToggle);
		}
	}
}

void AGHPlayerController::Look(const FInputActionValue& Value)
{
	// Input is a Vector2D
	auto LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AGHPlayerController::Move(const FInputActionValue& Value)
{
	// Input is a Vector2D
	const auto MovementVector = Value.Get<FVector2D>();

	// Find out which way is forward
	const auto Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Get forward vector
	const auto ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// Get right vector
	const auto RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (const auto PawnMoved = GetPawn())
	{
		// Add movement
		PawnMoved->AddMovementInput(ForwardDirection, MovementVector.Y);
		PawnMoved->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGHPlayerController::Jump(const FInputActionValue& Value)
{
	if (const auto CharacterJumping = Cast<ACharacter>(GetPawn()))
	{
		CharacterJumping->Jump();
	}
}

void AGHPlayerController::StopJumping(const FInputActionValue& Value)
{
	if (const auto CharacterJumping = Cast<ACharacter>(GetPawn()))
	{
		CharacterJumping->StopJumping();
	}
}

void AGHPlayerController::Inventory(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHPlayerController::Inventory");

	if (const auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		auto World = GEngine->GetCurrentPlayWorld(nullptr);
		auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

		GameMode->Inventory->ShowInventory(Npc);
	}
}

void AGHPlayerController::Dialog(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHPlayerController::Dialog");

	if (const auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		const auto World = GEngine->GetCurrentPlayWorld(nullptr);
		const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

		const auto OtherNpc = Cast<AGHNPC>(Helpers::GetNearestNPC());
		
		// GameMode->Dialog->Interact(Npc, OtherNpc);
	}
}

void AGHPlayerController::Stats(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHPlayerController::Stats");

	if (const auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		const auto World = GEngine->GetCurrentPlayWorld(nullptr);
		const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

		GameMode->Stats->Show(Npc);
	}
}

void AGHPlayerController::Quest(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHPlayerController::Quest");

	if (const auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		const auto World = GEngine->GetCurrentPlayWorld(nullptr);
		const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

		GameMode->Quest->Show();
	}
}

void AGHPlayerController::Posses(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHPlayerController::Posses");

	if (const auto PlayerPawn = Cast<AGHNPC>(GetPawn()))
	{
		const auto World = GEngine->GetCurrentPlayWorld(nullptr);
		const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

		// to Npc
		const auto NearestNpc = Helpers::GetNearestNPC();
		if (NearestNpc)
			Possess(NearestNpc);

		// free cam
		// if (GameMode->FreeCameraClass)
		// {
		// 	auto Position = GetPawn()->GetActorLocation();
		// 	auto Rotation = GetPawn()->GetActorRotation();
		// 	FTransform SpawnTransform(Rotation, Position, FVector(1,1,1));
		// 	auto FreeCamera = World->SpawnActor<APawn>(GameMode->FreeCameraClass, SpawnTransform);
		// 	if (FreeCamera)
		// 	{
		// 		Possess(FreeCamera);
		// 	}
		// }
	}
}



// left mouse
void AGHPlayerController::RotInc(const FInputActionValue& Value)
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	// GameMode->Rot(1);

	if (const auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->RemoveAnyWeapon();
	}
}

void AGHPlayerController::RotDec(const FInputActionValue& Value)
{
	// const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	//
	// GameMode->Rot(-1);

	if (const auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		// Npc->AnimInstance->PlayMontageTest();
		//Npc->Test_LoadSkeletalMesh();
	}
}

// right mouse
void AGHPlayerController::ChangeAxisCF(const FInputActionValue& Value)
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	// GameMode->ChangeAxis();
	
	if (const auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->ReadyMeleeWeapon();
		// Npc->MoveToWaypointDelayed();
		//Npc->Test_LoadAnimation();
		//Npc->Test_PlayAnimation();

		// Npc->AnimInstance->PlayMontageTest();
		// Npc->AnimInstance->PlayMontageByName("S_RUN");
		// Npc->AnimInstance->RunStateMachine();
		// Npc->AnimInstance->PlayMontageByName("S_RUNL");
	}
}



void AGHPlayerController::StartMoveForward(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StartMoveForward();
	}
}

void AGHPlayerController::StopMoveForward(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StopMoveForward();
	}
}

void AGHPlayerController::StartMoveBackward(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StartMoveBackward();
		// Npc->AnimInstance->PlayMontageTest();
	}
}

void AGHPlayerController::StopMoveBackward(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StopMoveBackward();
	}
}

void AGHPlayerController::StartMoveLeft(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StartMoveLeft();
	}
}

void AGHPlayerController::StopMoveLeft(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StopMoveLeft();
	}
}

void AGHPlayerController::StartMoveRight(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StartMoveRight();
	}
}

void AGHPlayerController::StopMoveRight(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StopMoveRight();
	}
}

void AGHPlayerController::StartTurnLeft(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StartTurnLeft();
	}
}

void AGHPlayerController::StopTurnLeft(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StopTurnLeft();
	}
}

void AGHPlayerController::StartTurnRight(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StartTurnRight();
	}
}

void AGHPlayerController::StopTurnRight(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->AnimInstance->StopTurnRight();
	}
}


void AGHPlayerController::MeleeWeaponToggle(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AGHPlayerController::MeleeWeaponToggle");
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->InputMeleeWeaponToggle();
	}
}

void AGHPlayerController::RangeWeaponToggle(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AGHPlayerController::RangeWeaponToggle");
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->InputRangeWeaponToggle();
	}
}

void AGHPlayerController::MagicWeaponToggle(const FInputActionValue& Value)
{
	if (auto Npc = Cast<AGHNPC>(GetPawn()))
	{
		Npc->InputMagicWeaponToggle();
	}
}