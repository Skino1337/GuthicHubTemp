// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHLevel.h"

#include "Game/GHGameMode.h"


// Sets default values
AGHLevel::AGHLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	Model = "";
}

// Called when the game starts or when spawned
void AGHLevel::BeginPlay()
{
	Super::BeginPlay();

	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	if (Model.Len() > 0)
	{
		GameMode->GLTFRuntime->LoadModel(StaticMeshComponent, Model, true);
		// StaticMeshComponent->SetSimulatePhysics(true);
	}
}

// Called every frame
void AGHLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

