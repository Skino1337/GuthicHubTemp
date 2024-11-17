// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHEntity.h"

#include "glTFRuntimeAssetActor.h"
#include "Game/GHGameMode.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGHEntity::AGHEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	//StaticMeshComponent->SetMassScale(NAME_None,1.0f);
	//StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	TooltipComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Tooltip"));
	TooltipComponent->SetupAttachment(StaticMeshComponent);
	TooltipComponent->SetWidgetSpace(EWidgetSpace::Screen);
	TooltipComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	
	static ConstructorHelpers::FClassFinder<UUserWidget> TooltipClass(TEXT("/Game/UI/EntityTooltip"));
	if (TooltipClass.Class != nullptr)
		TooltipComponent->SetWidgetClass(TooltipClass.Class);

	_table = FLuaValue();
}

// Called when the game starts or when spawned
void AGHEntity::BeginPlay()
{
	Super::BeginPlay();

	TooltipComponent->InitWidget();
	Tooltip = Cast<UGHEntityTooltip>(TooltipComponent->GetUserWidgetObject());
	
	FTimerHandle DelayedBeginPlayTimer;
	GetWorldTimerManager().SetTimer(DelayedBeginPlayTimer, this, &AGHEntity::DelayedBeginPlay, 0.1f, false);
}

void AGHEntity::DelayedBeginPlay() // Init from Table
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	if (_table.Type == ELuaValueType::Table)
	{
		FLuaValue Name = _table.GetField("name");
		if (Name.Type == ELuaValueType::String)
		{
			FString TranslatedString = Name.ToString();
			if (GameMode)
				TranslatedString = GameMode->GetTranslatedString(TranslatedString);
			ChangeTooltip(TranslatedString);
		}
		FLuaValue PhysicalWeight = _table.GetField("physicalWeight");
		if (PhysicalWeight.Type == ELuaValueType::Integer || PhysicalWeight.Type == ELuaValueType::Number)
			StaticMeshComponent->SetMassOverrideInKg(NAME_None, PhysicalWeight.ToFloat(), true);
		else
			StaticMeshComponent->SetMassOverrideInKg(NAME_None, 1, false);
		FLuaValue LuaValueModel = _table.GetField("model");
		if (LuaValueModel.Type == ELuaValueType::String)
		{
			GameMode->GLTFRuntime->LoadModel(StaticMeshComponent, LuaValueModel.ToString(), true);
			StaticMeshComponent->SetSimulatePhysics(true);
			// auto PathToModel = "C:/" + Model.ToString() + ".gltf";
			// FglTFRuntimeConfig LoaderConfig;
			// RuntimeAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromFilename(PathToModel,false, LoaderConfig);
			// if (RuntimeAsset)
			// {
			// 	FglTFRuntimeStaticMeshConfig StaticMeshConfig;
			// 	StaticMeshConfig.bBuildSimpleCollision = true;
			// 	auto StaticMesh = RuntimeAsset->LoadStaticMesh(0, StaticMeshConfig);
			// 	if (StaticMesh)
			// 	{
			// 		StaticMeshComponent->SetStaticMesh(StaticMesh);
			// 		StaticMeshComponent->SetSimulatePhysics(true);
			// 		// StaticMeshComponent->SetMassOverrideInKg(NAME_None, 1, false);
			// 	}
			
				// auto Transform = this->GetTransform();
				// Transform.SetScale3D(FVector(1.f, 1.f, 1.f));
				// FActorSpawnParameters SpawnInfo;
				//
				// auto SpawnedActor = World->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(), Transform);
				// if (SpawnedActor)
				// {
				// 	SpawnedActor->Asset = RuntimeAsset;
				// 	SpawnedActor->FinishSpawning(Transform);
				// 	auto AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
				// 	SpawnedActor->AttachToActor(this, AttachmentTransformRules);
				// }
			 //}
		}
	}
}

void AGHEntity::ChangeTooltip(FString Text)
{
	if (Tooltip)
		Tooltip->UpdateText(Text);
}

// Called every frame
void AGHEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

