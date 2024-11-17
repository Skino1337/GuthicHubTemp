// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "RuntimeAudioImporterLibrary.h"

#include "UI/GHDialogText.h"
#include "UI/GHDialogChoices.h"
//#include "UI/GHDialogChoicesButton.h"

#include "Game/GHNPC.h"
#include "Player/GHCharacter.h"
#include "Struct/GHDialogStruct.h"

#include "GHDialog.generated.h"

// Forward Declarations
class UGHDialogChoicesButton;

UENUM()
enum class EState
{
	None,
	Choices,
	Talk,
	ImportantTalk
};


UCLASS()
class GOTHICHUB_API UGHDialog : public UObject
{
	GENERATED_BODY()

public:
	UGHDialog();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWorld* World;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGHCharacter* PlayerLatest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGHNPC* NpcLatest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHDialogText* DialogText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> DialogTextClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHDialogChoices* DialogChoices;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> DialogChoicesClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHDialogChoicesButton* DialogChoicesButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> DialogChoicesButtonClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLuaValue LuaDialogThread;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FDialog CurrentDialog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class URuntimeAudioImporterLibrary* RuntimeAudioImporter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> DialogKnowInfoArray;
	
	UFUNCTION()
	void Initialization();
	
	UFUNCTION()
	void Interact(AGHCharacter* Player, AGHNPC* Npc);
	
	UFUNCTION()
	void DelayedPlayAudio(FString AudioPath);
	
	UFUNCTION()
	void DialogStart(FDialog Dialog);
	
	UFUNCTION()
	void CreateWidgetDialogText(FLuaValue Sources, FLuaValue Destination, FLuaValue Text);
	
	UFUNCTION()
	void EndWidgetDialogText();
};
