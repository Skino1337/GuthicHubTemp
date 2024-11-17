#pragma once

#include "CoreMinimal.h"

#include "Core/GHLuaState.h"
#include "Game/GHNPC.h"
#include "Game/GHGameMode.h"
#include "Player/GHCharacter.h"
#include "Core/GHLuaState.h"

class GOTHICHUB_API Helpers
{
public:
	static AGHNPC* GetNearestNPC();
	
	static FWaypoint GetNearestWaypoint(FVector Location);
	
	static FWaypoint* GetWaypointByName(const FString& WaypointName);

	static TArray<FString> CalculatePathToWaypoint(FVector StartLocation, FWaypoint TargetWaypoint);
	
	static bool NpcCheck(AGHNPC* Npc);

	static bool CheckInstance(FLuaValue Instance, FString Type = "");

	static bool RecheckGlobalInstance(const FString& InstanceName);

	static AGHNPC* GetNpcFromLuaValue(FLuaValue FLuaValueNpc);

	static AGHNPC* GetPlayerNpc();

	static FLuaValue GetGlobalLuaValue(const FString& Name);

	static FLuaValue CreateLuaObject(FLuaValue Instance);
};
