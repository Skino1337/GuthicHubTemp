#include "Game/NPC/GHNPCActionQueue.h"

#include "Game/GHAnimInstance.h"
#include "Game/GHNPC.h"


UGHNPCActionQueue::UGHNPCActionQueue()
{
	NPC = nullptr;
	ActionQueueStory = {};
	ActionQueue = {};
}

// ---
// Action: Weapon
// ---

void UGHNPCActionQueue::DrawWeaponMeleeActionAdd()
{
	auto WeaponMessage = NewObject<UWeaponMessage>(this);
	WeaponMessage->MessageWeapon = EMessageWeapon::DrawWeaponMelee;

	WeaponMessage->MessageInformation = "DrawWeaponMelee";
	ActionQueue.Add(WeaponMessage);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "DrawWeaponMeleeActionAdd");
}

void UGHNPCActionQueue::DrawWeaponMeleeActionExecute()
{
	// @TODO: check slots
	// @TODO: check fists

	if (!NPC)
	{
		DrawWeaponMeleeActionEnd();
		return;
	}
	
	if (!NPC->AnimInstance)
	{
		DrawWeaponMeleeActionEnd();
		return;
	}
	
	if (NPC->ItemInHands.Type == ELuaValueType::Table)
	{
		DrawWeaponMeleeActionEnd();
		return;
	}

	if (NPC->EquippedMeleeWeapon.Type != ELuaValueType::Table)
	{
		DrawWeaponMeleeActionEnd();
		return;
	}

	auto LuaValueWeaponCategory = NPC->EquippedMeleeWeapon.GetField("mainflag");
	if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer ||
		LuaValueWeaponCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer|Number"));
		DrawWeaponMeleeActionEnd();
		return;
	}

	auto LuaValueWeaponSubcategory = NPC->EquippedMeleeWeapon.GetField("flags");
	if (!(LuaValueWeaponSubcategory.Type == ELuaValueType::Integer ||
		LuaValueWeaponSubcategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponSubcategory.Type != ELuaValueType::Integer|Number"));
		return;
	}

	// FString WeaponSwapSocketName;
	
	TArray<FString> AnimationReadyWeapon = {""};
	auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
	if (WeaponCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		auto WeaponSubcategory = LuaValueWeaponSubcategory.ToInteger();
		if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Sword))
		{
			NPC->WeaponSwapEquipSocketName = "SWORD";
			NPC->WeaponSwapHandSocketName = "RIGHTHAND";
			
			if (NPC->AnimInstance->BodyState == EBodyState::Run || NPC->AnimInstance->BodyState == EBodyState::Walk)
				AnimationReadyWeapon[0] = "T_MOVE_2_1HMOVE";
			else
				AnimationReadyWeapon = {"T_RUN_2_1H", "S_1H", "T_1H_2_1HRUN"};
		}
		else if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Axe))
		{
			NPC->WeaponSwapEquipSocketName = "SWORD";
			NPC->WeaponSwapHandSocketName = "RIGHTHAND";
			
			if (NPC->AnimInstance->BodyState == EBodyState::Run || NPC->AnimInstance->BodyState == EBodyState::Walk)
				AnimationReadyWeapon[0] = "T_MOVE_2_1HMOVE";
			else
				AnimationReadyWeapon = {"T_RUN_2_1H", "S_1H", "T_1H_2_1HRUN"};
			// sound = axe "T_1HMOVE_2_MOVE"; axe have own sound...
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong EItemSubcategory"));
			DrawWeaponMeleeActionEnd();
			return;
		}
		//
		// if (WeaponSwapSocketName.IsEmpty())
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("WeaponSwapSocketName.IsEmpty()"));
		// 	return;
		// }
		//
		// WeaponSwapSocketName = WeaponEquipStaticMeshComp;
	}

	if (AnimationReadyWeapon.Num() == 1)
	{
		if (!AnimationReadyWeapon[0].IsEmpty())
		{
			NPC->AnimInstance->PlayMontageByName(AnimationReadyWeapon[0], false);
		}
	}
	else if (AnimationReadyWeapon.Num() > 1)
	{
		NPC->AnimInstance->PlayMontageQueue(AnimationReadyWeapon);
	}
}

void UGHNPCActionQueue::DrawWeaponMeleeActionEnd()
{
	if (ActionQueue.Num() <= 0)
		return;

	if (ActionQueue[0]->MessageType != EActionType::Weapon)
		return;
	
	auto WeaponMessage = Cast<UWeaponMessage>(ActionQueue[0]);
	if (!WeaponMessage)
		return;

	if (WeaponMessage->MessageWeapon != EMessageWeapon::DrawWeaponMelee)
		return;

	RemoveActionFromQueue();

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "DrawWeaponMeleeActionEnd");
}

// ---

void UGHNPCActionQueue::DrawWeaponRangeActionAdd()
{
	auto WeaponMessage = NewObject<UWeaponMessage>(this);
	WeaponMessage->MessageWeapon = EMessageWeapon::DrawWeaponRange;

	WeaponMessage->MessageInformation = "DrawWeaponRange";
	ActionQueue.Add(WeaponMessage);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "DrawWeaponRangeActionAdd");
}

void UGHNPCActionQueue::DrawWeaponRangeActionExecute()
{
	// @TODO: check slots
	// @TODO: check fists

	if (!NPC)
	{
		DrawWeaponRangeActionEnd();
		return;
	}
	
	if (!NPC->AnimInstance)
	{
		DrawWeaponRangeActionEnd();
		return;
	}
	
	if (NPC->ItemInHands.Type == ELuaValueType::Table)
	{
		DrawWeaponRangeActionEnd();
		return;
	}
	
	if (NPC->EquippedRangeWeapon.Type != ELuaValueType::Table)
	{
		DrawWeaponRangeActionEnd();
		return;
	}

	auto LuaValueWeaponCategory = NPC->EquippedRangeWeapon.GetField("mainflag");
	if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer ||
		LuaValueWeaponCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer|Number"));
		DrawWeaponRangeActionEnd();
		return;
	}

	auto LuaValueWeaponSubcategory = NPC->EquippedRangeWeapon.GetField("flags");
	if (!(LuaValueWeaponSubcategory.Type == ELuaValueType::Integer ||
		LuaValueWeaponSubcategory.Type == ELuaValueType::Number))
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueWeaponSubcategory.Type != ELuaValueType::Integer|Number"));
		DrawWeaponRangeActionEnd();
		return;
	}

	auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
	auto WeaponSubcategory = LuaValueWeaponSubcategory.ToInteger();
	
	TArray<FString> DrawWeaponAnimationArray = {""};
	if (WeaponCategory & static_cast<int32>(EItemCategory::RangeWeapon))
	{
		if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Bow))
		{
			NPC->WeaponSwapEquipSocketName = "BOW";
			NPC->WeaponSwapHandSocketName = "LEFTHAND";
			
			if (NPC->AnimInstance->BodyState == EBodyState::Run || NPC->AnimInstance->BodyState == EBodyState::Walk)
				DrawWeaponAnimationArray[0] = "T_MOVE_2_BOWMOVE";
			else
				DrawWeaponAnimationArray = {"T_RUN_2_BOW", "S_BOW", "T_BOW_2_BOWRUN"};
		}
		else if (WeaponSubcategory & static_cast<int32>(EItemSubcategory::Crossbow))
		{
			// WeaponSwapEquipSocketName = "SWORD";
			// WeaponSwapHandSocketName = "RIGHTHAND";
			//
			// if (AnimInstance->BodyState == EBodyState::Run || AnimInstance->BodyState == EBodyState::Walk)
			// 	DrawWeaponAnimationArray[0] = "T_MOVE_2_1HMOVE";
			// else
			// 	DrawWeaponAnimationArray = {"T_RUN_2_1H", "S_1H", "T_1H_2_1HRUN"};
			// // sound = axe "T_1HMOVE_2_MOVE"; axe have own sound...
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong EItemSubcategory"));
			DrawWeaponRangeActionEnd();
			return;
		}
		//
		// if (WeaponSwapSocketName.IsEmpty())
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("WeaponSwapSocketName.IsEmpty()"));
		// 	return;
		// }
		//
		// WeaponSwapSocketName = WeaponEquipStaticMeshComp;
	}

	if (DrawWeaponAnimationArray.Num() == 1)
	{
		if (!DrawWeaponAnimationArray[0].IsEmpty())
		{
			NPC->AnimInstance->PlayMontageByName(DrawWeaponAnimationArray[0], false);
		}
	}
	else if (DrawWeaponAnimationArray.Num() > 1)
	{
		NPC->AnimInstance->PlayMontageQueue(DrawWeaponAnimationArray);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "PlayMontageQueue");
	}
}

void UGHNPCActionQueue::DrawWeaponRangeActionEnd()
{
	if (ActionQueue.Num() <= 0)
		return;

	if (ActionQueue[0]->MessageType != EActionType::Weapon)
		return;
	
	auto WeaponMessage = Cast<UWeaponMessage>(ActionQueue[0]);
	if (!WeaponMessage)
		return;

	if (WeaponMessage->MessageWeapon != EMessageWeapon::DrawWeaponRange)
		return;

	RemoveActionFromQueue();
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "DrawWeaponRangeActionEnd");
}

// ---

void UGHNPCActionQueue::RemoveWeaponMeleeActionAdd()
{
	auto WeaponMessage = NewObject<UWeaponMessage>(this);
	WeaponMessage->MessageWeapon = EMessageWeapon::RemoveWeapon;
	
	WeaponMessage->MessageInformation = "RemoveWeapon";
	ActionQueue.Add(WeaponMessage);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "RemoveWeaponMeleeActionAdd");
}

void UGHNPCActionQueue::RemoveWeaponMeleeActionExecute()
{
	NPC->RemoveAnyWeapon();
}

void UGHNPCActionQueue::RemoveWeaponMeleeActionEnd()
{
	if (ActionQueue.Num() <= 0)
		return;

	if (ActionQueue[0]->MessageType != EActionType::Weapon)
		return;
	
	auto WeaponMessage = Cast<UWeaponMessage>(ActionQueue[0]);
	if (!WeaponMessage)
		return;

	if (WeaponMessage->MessageWeapon != EMessageWeapon::RemoveWeapon)
		return;

	RemoveActionFromQueue();
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "RemoveWeaponMeleeActionEnd");
}

// ---

void UGHNPCActionQueue::RemoveWeaponRangeActionAdd()
{
	auto WeaponMessage = NewObject<UWeaponMessage>(this);
	WeaponMessage->MessageWeapon = EMessageWeapon::RemoveWeapon;

	WeaponMessage->MessageInformation = "RemoveWeapon";
	ActionQueue.Add(WeaponMessage);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "RemoveWeaponRangeActionAdd");
}

void UGHNPCActionQueue::RemoveWeaponRangeActionExecute()
{
	NPC->RemoveAnyWeapon();
}

void UGHNPCActionQueue::RemoveWeaponRangeActionEnd()
{
	if (ActionQueue.Num() <= 0)
		return;

	if (ActionQueue[0]->MessageType != EActionType::Weapon)
		return;
	
	auto WeaponMessage = Cast<UWeaponMessage>(ActionQueue[0]);
	if (!WeaponMessage)
		return;

	if (WeaponMessage->MessageWeapon != EMessageWeapon::RemoveWeapon)
		return;

	RemoveActionFromQueue();
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "RemoveWeaponRangeActionEnd");
}

// ---

void UGHNPCActionQueue::WeaponActionCheck(UWeaponMessage* WeaponMessage)
{
	if (WeaponMessage->MessageWeapon == EMessageWeapon::DrawWeaponMelee)
	{
		if (WeaponMessage->bInUse == false)
		{
			WeaponMessage->bInUse = true;
			DrawWeaponMeleeActionExecute();
		}
	}
	else if (WeaponMessage->MessageWeapon == EMessageWeapon::DrawWeaponRange)
	{
		if (WeaponMessage->bInUse == false)
		{
			WeaponMessage->bInUse = true;
			DrawWeaponRangeActionExecute();
		}
	}
	else if (WeaponMessage->MessageWeapon == EMessageWeapon::RemoveWeapon)
	{
		if (WeaponMessage->bInUse == false)
		{
			WeaponMessage->bInUse = true;
			RemoveWeaponMeleeActionExecute();
		}
	}
}

// ---
// Action: Weapon
// ---


// ---
// Action: UseItem
// ---

void UGHNPCActionQueue::EquipItemActionAdd(const FString& ItemInstance)
{
	auto UseItemMessage = NewObject<UUseItemMessage>(this);
	UseItemMessage->MessageUseItem = EMessageUseItem::EquipItem;
	UseItemMessage->InstanceItem = ItemInstance;

	UseItemMessage->MessageInformation = "EquipItem(" + ItemInstance + ")";
	ActionQueue.Add(UseItemMessage);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "EquipItemActionAdd");
}

void UGHNPCActionQueue::EquipItemActionExecute(const FString& ItemInstanceName)
{
	auto LuaValueItems = NPC->_table.GetField("items");
	if (LuaValueItems.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueItems.Type != ELuaValueType::Table"));
		return;
	}

	// @TODO: check is item equpped?
	
	auto LuaValueInventoryItemArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(LuaValueItems);
	for (auto LuaValueItem : LuaValueInventoryItemArray)
	{
		if (LuaValueItem.Type != ELuaValueType::Table)
			continue;

		auto InstanceFieldName = LuaValueItem.GetField("__instance").ToString();
		if (ItemInstanceName == InstanceFieldName)
		{
			auto LuaValueEquipped = LuaValueItem.GetField("equipped");
			if (LuaValueEquipped.Type != ELuaValueType::Bool)
			{
				UE_LOG(LogTemp, Warning, TEXT("LuaValueEquipped.Type != ELuaValueType::Bool"));
				return;
			}

			if (LuaValueEquipped.ToBool())
			{
				// already have and equipped
				return;
			}
			else
			{
				// only add mesh
				NPC->EquipMesh(LuaValueItem);
				return;
			}
		}
	}

	auto LuaValueItemInstance = Helpers::GetGlobalLuaValue(ItemInstanceName);
	if (!Helpers::CheckInstance(LuaValueItemInstance, C_ITEM))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s it's not item instance"), *ItemInstanceName);
		return;
	}
	
	auto LuaValueCreatedItem = NPC->InventoryCreateItems(LuaValueItemInstance, 1);
	if (LuaValueCreatedItem.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("LuaValueCreatedItem.Type != ELuaValueType::Table"));
		return;
	}
	
	NPC->EquipMesh(LuaValueCreatedItem);

	EquipItemActionEnd();
}

void UGHNPCActionQueue::EquipItemActionEnd()
{
	if (ActionQueue.Num() <= 0)
		return;

	if (ActionQueue[0]->MessageType != EActionType::UseItem)
		return;
	
	auto UseItemMessage = Cast<UUseItemMessage>(ActionQueue[0]);
	if (!UseItemMessage)
		return;

	if (UseItemMessage->MessageUseItem != EMessageUseItem::EquipItem)
		return;

	RemoveActionFromQueue();
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "EquipItemActionEnd");
}

// ---

void UGHNPCActionQueue::UseItemActionCheck(UUseItemMessage* UseItemMessage)
{
	if (UseItemMessage->MessageUseItem == EMessageUseItem::EquipItem)
	{
		EquipItemActionExecute(UseItemMessage->InstanceItem);
	}
}

// ----
// Action: UseItem
// ----



void UGHNPCActionQueue::RemoveActionFromQueue()
{
	if (ActionQueue.Num() == 0)
		return;

	if (ActionQueueStory.Num() >= 10)
		ActionQueueStory.RemoveAt(0, 1, false);

	// тут вроде всё ок
	ActionQueueStory.Add(ActionQueue[0]);
	ActionQueue.RemoveAt(0, 1, false);
}

void UGHNPCActionQueue::ExecuteActionQueue()
{
	if (!NPC)
		return;
	
	if (ActionQueue.Num() <= 0)
		return;
	
	if (ActionQueue[0]->MessageType == EActionType::Weapon)
	{
		auto WeaponMessage = Cast<UWeaponMessage>(ActionQueue[0]);
		if (WeaponMessage)
		{
			WeaponActionCheck(WeaponMessage);
		}
	}
	else if (ActionQueue[0]->MessageType == EActionType::UseItem)
	{
		auto UseItemMessage = Cast<UUseItemMessage>(ActionQueue[0]);
		if (UseItemMessage)
		{
			UseItemActionCheck(UseItemMessage);
		}
	}
}