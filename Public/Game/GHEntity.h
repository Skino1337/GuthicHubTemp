// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"

#include "UI/GHEntityTooltip.h"

#include "LuaBlueprintFunctionLibrary.h"
#include "glTFRuntimeFunctionLibrary.h"

#include "GHEntity.generated.h"


UCLASS()
class GOTHICHUB_API AGHEntity : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AGHEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHEntityTooltip* Tooltip;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* TooltipComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UglTFRuntimeAsset* RuntimeAsset;
	
	//
	// // --
	// // Gothic game structure (updated)
	// // http://www.gothic-library.ru/publ/class_c_item/1-1-0-34
	// // --
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString __type;
	//
	// // custom
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// bool equiped;
	//
	// // custom
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 amount;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 id;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString name;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString nameID;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 hp;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 hp_max;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 mainflag;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 flags;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 weight;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 value;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 damageType;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 damageTotal;
	//
	// // UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// // int32 damage[DAM_INDEX_MAX];
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 wear;
	//
	// // // int32 protection[PROT_INDEX_MAX];
	// // UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// // int32 protection[PROT_INDEX_MAX];
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 nutrition;
	//
	// // int32 cond_atr[3];
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue cond_atr;
	//
	// // UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// // int32 cond_value[3];
	// //
	// // UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// // int32 change_atr[3];
	// //
	// // UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// // int32 change_value[3];
	//
	// // func
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue magic;
	//
	// // func
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue on_equip;
	//
	// // func
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue on_unequip;
	//
	// // func on_state[4]
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue on_state;
	//
	// // func
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue owner;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 ownerGuild;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 disguiseGuild;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString visual;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString visual_change;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString effect;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 visual_skin;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString scemeName;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 material;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 munition;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 spell;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 range;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 mag_circle;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FString description;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TArray<FString> text;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TArray<int32> count;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 inv_zbias;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 inv_rotx;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 inv_roty;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 inv_rotz;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 inv_animate;
	//
	// // custom
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FLuaValue _table;
	//
	// // --
	// // 
	// // --

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLuaValue _table;

	UFUNCTION()
	void DelayedBeginPlay();
	
	UFUNCTION()
	void ChangeTooltip(FString Text);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
