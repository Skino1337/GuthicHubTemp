#include "Game/Vobs/GHVob.h"
#include "Game/GHGameMode.h"


// Sets default values
AGHVob::AGHVob()
{
	// DO NOT CHANGE, zavisaet
	// Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// Root->SetupAttachment(RootComponent); // This log me for unreal alive
	//SetRootComponent(Root);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// Root->SetupAttachment(RootComponent);
	SetRootComponent(Root);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(Root);
	StaticMeshComponent->SetRelativeRotation(FRotator(0, 90, 0));

	NameTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameTextRender"));
	NameTextRenderComponent->SetupAttachment(Root);
	NameTextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	NameTextRenderComponent->SetText(FText::FromString(""));
	NameTextRenderComponent->SetVisibility(false);
	NameTextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;

	PosTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PosTextRender"));
	PosTextRenderComponent->SetupAttachment(Root);
	PosTextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	PosTextRenderComponent->SetText(FText::FromString(""));
	PosTextRenderComponent->SetVisibility(false);
	PosTextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;

	DirTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DirTextRender"));
	DirTextRenderComponent->SetupAttachment(Root);
	DirTextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	DirTextRenderComponent->SetText(FText::FromString(""));
	DirTextRenderComponent->SetVisibility(false);
	DirTextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AGHVob::BeginPlay()
{
	Super::BeginPlay();

	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	// // compiLATION ERROR, this not for build need if in editor?
	// if (ZenStructVob.VobName.Len() > 0)
	// 	SetActorLabel(ZenStructVob.VobName);
	
	if (ZenStructVob.ShowVisual && ZenStructVob.Visual.Len() > 0)
	{
		FString Name, Format;
		ZenStructVob.Visual.Split(".",&Name, &Format);

		if (Format.Contains("3DS"))
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "model before: " + ZenStructVob.Visual);
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "model after: " + Name);

			// auto StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
			// StaticMeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
			// StaticMeshComponent->RegisterComponent();
			
			auto IsModelLoaded = GameMode->GLTFRuntime->LoadModel(StaticMeshComponent, Name, false);
			if (IsModelLoaded)
			{
				StaticMeshComponent->SetRelativeRotation(FRotator(90, 0, 0));

				NameTextRenderComponent->SetText(FText::FromString(ZenStructVob.Visual));
				NameTextRenderComponent->SetVisibility(true);
	
				auto PosText = "X: " + FString::FromInt(ZenStructVob.TrafoOSToWSPos.X);
				PosText += ", Y: " + FString::FromInt(ZenStructVob.TrafoOSToWSPos.Y);
				PosText += ", Z: " + FString::FromInt(ZenStructVob.TrafoOSToWSPos.Z);
				PosTextRenderComponent->SetText(FText::FromString(PosText));
				PosTextRenderComponent->SetVisibility(true);

				auto DirText = "Roll: " + FString::SanitizeFloat(ZenStructVob.TrafoOSToWSRot.Roll);
				DirText += ", Pitch: " + FString::SanitizeFloat(ZenStructVob.TrafoOSToWSRot.Pitch);
				DirText += ", Yaw: " + FString::SanitizeFloat(ZenStructVob.TrafoOSToWSRot.Yaw);
				DirTextRenderComponent->SetText(FText::FromString(DirText));
				DirTextRenderComponent->SetVisibility(true);
			}
		}
	}
}