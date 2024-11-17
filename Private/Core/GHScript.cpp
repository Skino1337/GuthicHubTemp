#include "Core/GHScript.h"

#include "Core/GHLuaState.h"


UGHScript::UGHScript()
{
//	Table.Add("Wld_InsertItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_InsertItem)));
	Table.Add("Wld_InsertNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_InsertNpc)));
}

// void UGHScript::Initialization(UGHLuaState* LuaState)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHScript::Initialization");
// }

void UGHScript::Wld_InsertItem(FLuaValue ItemInstance, FLuaValue SpawnPoint)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHScript::Wld_InsertItem");
}