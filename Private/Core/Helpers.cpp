
#include "Core/Helpers.h"

#include "Kismet/GameplayStatics.h"


AGHNPC* Helpers::GetNearestNPC()
{
	UWorld* World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return nullptr;
	
	AGHGameMode* GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode)
		return nullptr;

	const auto PlayerCharacter = Cast<AGHNPC>(UGameplayStatics::GetPlayerCharacter(World, 0));
	if (!PlayerCharacter)
		return nullptr;

	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	float MinDist = 0.0f;
	AGHNPC* ClosestNpc = nullptr;
	for (auto Npc : GameMode->NpcArray)
	{
		if (PlayerCharacter == Npc)
			continue;
		
		FVector NpcLocation = Npc->GetActorLocation();
		float Dist = FVector::Dist(PlayerLocation, NpcLocation);
		if (MinDist == 0.0f)
		{
			MinDist = Dist;
			ClosestNpc = Npc;
		}
		else
		{
			if (Dist < MinDist)
			{
				MinDist = Dist;
				ClosestNpc = Npc;
			}
		}
	}

	return ClosestNpc;
}

FWaypoint Helpers::GetNearestWaypoint(FVector Location)
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	auto MinDist = 0.0f;
	auto NearestWaypoint = FWaypoint();
	for (auto Waypoint : GameMode->WaypointMap)
	{
		float Dist = FVector::Dist(Location, Waypoint.Value.Position);
		if (MinDist == 0.0f)
		{
			MinDist = Dist;
			NearestWaypoint = Waypoint.Value;
		}
		else
		{
			if (Dist < MinDist)
			{
				MinDist = Dist;
				NearestWaypoint = Waypoint.Value;
			}
		}
	}

	return NearestWaypoint;
}

FWaypoint* Helpers::GetWaypointByName(const FString& WaypointName)
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	for (auto& Waypoint : GameMode->WaypointMap)
	{
		if (Waypoint.Value.Name == WaypointName)
			return &Waypoint.Value;
	}

	return nullptr;
}

TArray<FString> Helpers::CalculatePathToWaypoint(FVector StartLocation, FWaypoint TargetWaypoint)
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	auto NearestWaypoint = GetNearestWaypoint(StartLocation);

	// alg
	TArray<FWaypoint> Reachable = {NearestWaypoint};
	TArray<FWaypoint> Explored = {};
	
	while (Reachable.Num() > 0)
	{
		FWaypoint Waypoint;
		for (int32 i = 0; i < Reachable.Num(); i++)
		{
			if (Explored.Contains(Reachable[i]))
				continue;
	
			Waypoint = Reachable[i];
		}
	
		if (Waypoint.Name == TargetWaypoint.Name)
		{
			// function build_path (to_node):
			// 	path = []
			// 	while to_node != None:
			// 		path.add(to_node)
			// 		to_node = to_node.previous
			// 	return path
			
			FString Previous = "";
			TArray<FString> Path = {};
			
			Previous = Waypoint.Previous;
			if (Previous.Len() <= 0)
			{
				Path.Add(Waypoint.Name);
				return Path;
			}
			if (Previous.Len() > 0) // else now work...
			{
				Path.Add(Waypoint.Name);
			}
			
			while (!Previous.IsEmpty())
			{
				for (auto WaypointExplored : Explored)
				{
					if (WaypointExplored.Name == Previous)
					{
						Path.Add(WaypointExplored.Name);
						Previous = WaypointExplored.Previous;
					}
				}
			}
			return Path;
		}
			
		
		Explored.Add(Waypoint);
		Reachable.Remove(Waypoint);
	
		TArray<FWaypoint> Linked = {};
		for (auto WaypointLinkedName : Waypoint.Links)
		{
			if (GameMode->WaypointMap.Contains(WaypointLinkedName))
			{
				auto LinkedWaypoint = GameMode->WaypointMap.FindChecked(WaypointLinkedName);
				if (Explored.Contains(LinkedWaypoint))
					continue;
				
				Linked.Add(LinkedWaypoint);
			}
		}
	
		for (auto LinkedWaypoint : Linked)
		{
			if (!Reachable.Contains(LinkedWaypoint))
			{
				// TODO: opt
				LinkedWaypoint.Previous = Waypoint.Name;
				
				Reachable.Add(LinkedWaypoint);
			}
		}
	}

	return TArray<FString> {};
}

bool Helpers::NpcCheck(AGHNPC* Npc)
{
	if (!IsValid(Npc))
		return false;

	FLuaValue NpcTable = Npc->_table;
	if (NpcTable.Type != ELuaValueType::Table)
		return false;

	FLuaValue TypeField = NpcTable.GetField("__type");
	if (TypeField.Type != ELuaValueType::String)
		return false;

	if (TypeField.ToString() != "C_Npc")
		return false;

	FLuaValue InstanceField = NpcTable.GetField("__instance");
	if (InstanceField.Type != ELuaValueType::String)
		return false;

	return true;
}

bool Helpers::CheckInstance(FLuaValue Instance, FString Type)
{
	if (Instance.Type != ELuaValueType::Table)
		return false;
	
	if (Type.IsEmpty())
		Type = "C_";
	
	FLuaValue TypeField = Instance.GetField("__type");
	if (TypeField.Type != ELuaValueType::String)
		return false;

	if (!TypeField.ToString().StartsWith(Type))
		return false;

	FLuaValue InstanceField = Instance.GetField("__instance");
	if (InstanceField.Type != ELuaValueType::String)
		return false;

	if (InstanceField.ToString().IsEmpty())
		return false;

	return true;
}

bool Helpers::RecheckGlobalInstance(const FString& InstanceName)
{
	if (InstanceName.IsEmpty())
		return false;

	auto LuaValueInstance = GetGlobalLuaValue(InstanceName);
	
	if (!CheckInstance(LuaValueInstance))
		return false;

	if (InstanceName != LuaValueInstance.GetField("__instance").ToString())
		return false;

	return true;
}

AGHNPC* Helpers::GetNpcFromLuaValue(FLuaValue FLuaValueNpc)
{
	if (FLuaValueNpc.Type != ELuaValueType::UObject)
		return nullptr;
	
	auto Npc = Cast<AGHNPC>(FLuaValueNpc.Object);
	if (!Npc)
		return nullptr;
	
	auto IsValidNpc = NpcCheck(Npc);
	if (!IsValidNpc)
		return nullptr;

	return Npc;
}

AGHNPC* Helpers::GetPlayerNpc()
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	return Cast<AGHNPC>(UGameplayStatics::GetPlayerCharacter(World, 0));
}

FLuaValue Helpers::GetGlobalLuaValue(const FString& Name)
{
	auto ReturnLuaValue = FLuaValue();
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return ReturnLuaValue;
	
	ReturnLuaValue = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, UGHLuaState::StaticClass(), Name);

	return ReturnLuaValue;
}

FLuaValue Helpers::CreateLuaObject(FLuaValue Instance)
{
	auto ReturnLuaValue = FLuaValue();
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
		return ReturnLuaValue;
	
	auto LuaValueFunction = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, UGHLuaState::StaticClass(),
		"create_lua_object");
	if (LuaValueFunction.Type != ELuaValueType::Function)
		return ReturnLuaValue;

	auto LuaValueNewInstance = ULuaBlueprintFunctionLibrary::LuaGlobalCallValue(World,
		UGHLuaState::StaticClass(), LuaValueFunction, {Instance});
	if (LuaValueNewInstance.Type == ELuaValueType::Nil)
		return ReturnLuaValue;

	return LuaValueNewInstance;
}
