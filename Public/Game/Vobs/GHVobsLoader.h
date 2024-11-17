#pragma once

#include "CoreMinimal.h"

#include "Game/GHGameMode.h"

#include "Game/Vobs/GHVob.h"
#include "Game/Vobs/GHVobLight.h"


class GOTHICHUB_API GHVobsLoader
{
public:
	static void FillVobStruct(FVobStruct &Struct,  TSharedPtr<FJsonObject> JsonObject);
	static void FillVobLightStruct(FVobLightStruct &Struct, TSharedPtr<FJsonObject> JsonObject);
	
	static void LoadVobsFile();
};