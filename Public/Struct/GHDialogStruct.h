#pragma once


#include "CoreMinimal.h"

#include "LuaValue.h"

#include "GHDialogStruct.generated.h"


USTRUCT(BlueprintType)
struct FDialog
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	float Nr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool Important;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FLuaValue Condition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FLuaValue Information;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool Trade;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Struct")
	bool Permanent;

	FDialog()
	{
		Name = "";
		Nr = 1.0f;
		Important = false;
		Condition = FLuaValue();
		Information = FLuaValue();
		Description = "";
		Trade = false;
		Permanent = false;
	}
};
