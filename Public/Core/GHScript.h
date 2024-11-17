// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "LuaValue.h"
#include "LuaState.h"

#include "GHScript.generated.h"

/**
 * 
 */
UCLASS()
class GOTHICHUB_API UGHScript : public ULuaState
{
	GENERATED_BODY()

public:
	UGHScript();
	
	// void Initialization(UGHLuaState* LuaState);
	
	UFUNCTION(BlueprintCallable, Category = "Script")
	static void Wld_InsertItem(FLuaValue ItemInstance, FLuaValue SpawnPoint);
};
