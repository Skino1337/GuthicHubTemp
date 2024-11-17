#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GHEnums.generated.h"

UENUM() // BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true")
enum class EItemCategory: int32
{
	None				= 1 << 0,
	MeleeWeapon			= 1 << 1,
	RangeWeapon			= 1 << 2,
	Munition			= 1 << 3,
	Armor				= 1 << 4,
	Food				= 1 << 5,
	Docs				= 1 << 6,
	Potions				= 1 << 7,
	Light				= 1 << 8,
	Rune				= 1 << 9,
	Magic				= 1 << 31,
	Keys				= 1 << 0
};
// ENUM_CLASS_FLAGS(EItemCategory);

UENUM()
enum class EItemSubcategory: int32
{
	Burn				= 1 << 10,
	Mission				= 1 << 12,
	Multi				= 1 << 21,
	Torch				= 1 << 28,
	Throw				= 1 << 29,
	Sword				= 1 << 14,
	Axe					= 1 << 15,
	TwoHandedSword		= 1 << 16,
	TwoHandedAxe		= 1 << 17,
	Bow					= 1 << 19,
	Crossbow			= 1 << 20,
	Amulet				= 1 << 22,
	Ring				= 1 << 11
};

UENUM()
enum class ELoopResult: int32
{
	Continue			= 0,
	End					= 1
};

UENUM()
enum class EFightMode: int32
{
	None				= 0,
	Fist				= 1,
	Melee				= 2,
	Far					= 5,
	Magic				= 7
};

UENUM()
enum class EWalkMode: int32
{
	Run					= 0,
	Walk				= 1,
	Sneak				= 2,
	RunWeapon			= 128,
	WalkWeapon			= 129,
	SneakWeapon			= 130
};

UENUM()
enum class ETalent: int32
{
	OneHanded			= 1,
	TwoHanded			= 2,
	Bow					= 3,
	Crossbow			= 4,
	// ---
	Picklock			= 5,
	Mage				= 7,
	Sneak				= 8,
	Regenerate			= 9,
	Firemaster			= 10,
	Acrobat				= 11,
	// ---
	Pickpocket			= 12,
	Smith				= 13,
	Runes				= 14,
	Alchemy				= 15,
	TakeAnimalTrophy	= 16,
	ForeignLanguage		= 17,
	WispDetector		= 18,
	C					= 19,
	D					= 20,
	E					= 21,
	Max					= 22
};

UENUM()
enum class EAttitude: int32
{
	Hostile				= 0,
	Angry				= 1,
	Neutral				= 2,
	Friendly			= 3
};
