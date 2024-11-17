#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// #include "Game/GHAnimInstance.h"

#include "UI/DebugPanel/GHDebugPanel.h"
#include "UI/DebugPanel/GHAnimationStoryLine.h"


#include "GHDebug.generated.h"


// Forward Declarations
class UGHDebugPanel;


UCLASS()
class GOTHICHUB_API UGHDebug : public UObject
{
	GENERATED_BODY()

public:
	UGHDebug();
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNumberFormattingOptions NumberFormatOptions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGHDebugPanel* DebugPanel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> DebugPanelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> AnimationStoryLineClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> ActionQueueLineClass;
	
	UFUNCTION()
	void Initialization();

	UFUNCTION()
	void Show();

	UFUNCTION()
	void Update();

	UFUNCTION()
	void DebugStringUpdate(AGHNPC* Npc);
	
	UFUNCTION()
	void AnimationHistotyUpdate(AGHNPC* Npc);

	UFUNCTION()
	void AnimationElapsedUpdate(float Elapsed);
	
	UFUNCTION()
	void AnimationStartUpdate(const FString& AnimationName);
	
	UFUNCTION()
	void AnimationEndUpdate(float Elapsed, bool bInterrupted);

	UFUNCTION()
	void ActionQueueStoryUpdate(AGHNPC* NPC);
	
	UFUNCTION()
	void ActionQueueUpdate(AGHNPC* NPC);
};
