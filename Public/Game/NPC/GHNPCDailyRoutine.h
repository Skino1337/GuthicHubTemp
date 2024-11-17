// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// ---

// #include

// ---

#include "GHNPCDailyRoutine.generated.h"


UCLASS()
class GOTHICHUB_API UGHNPCDailyRoutine: public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartHour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StartMinute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StopHour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StopMinute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ScriptStateName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WaypointName;

	UGHNPCDailyRoutine()
	{
		StartHour = 0;
		StartMinute = 0;
		StopHour = 0;
		StopMinute = 0;
		ScriptStateName = "";
		WaypointName = "";
	}
};

//
// USTRUCT(BlueprintType)
// struct FDailyRoutine
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	int32 StartHour;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	int32 StartMinute;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	int32 StopHour;
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	int32 StopMinute;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	FString ScriptStateName;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
// 	FString WaypointName;
//
// 	FDailyRoutine()
// 	{
// 		StartHour = 0;
// 		StartMinute = 0;
// 		StopHour = 0;
// 		StopMinute = 0;
// 		ScriptStateName = "";
// 		WaypointName = "";
// 	}
// };