// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Vobs/GHVobLight.h"

#include "Components/SpotLightComponent.h"
#include "Game/GHGameMode.h"


// Called when the game starts or when spawned
void AGHVobLight::BeginPlay()
{
	Super::BeginPlay();

	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	if (ZenStructVobLight.LightType == ELightType::Point)
	{
		const auto PointLightComponent = NewObject<UPointLightComponent>(this);
		// PointLightComponent->LightColor = ZenStructVobLight.Color;
		// PointLightComponent->AttenuationRadius = ZenStructVobLight.Range;
		PointLightComponent->Intensity = 100000.0f;
		PointLightComponent->LightColor = FColor(255, 0, 0);
		PointLightComponent->AttenuationRadius = 16384.0f;
		//PointLightComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		PointLightComponent->RegisterComponent();
		
		// PointLightComponent->Intensity = 1000.0f;
		// PointLightComponent->AttenuationRadius = 20.f;
		// PointLightComponent->LightColor = FColor(0.0f, 1.0f, 0.0f);
		// PointLightComponent->SetVisibility(true);
		// PointLightComponent->bAutoActivate = true;
		// PointLightComponent->SetupAttachment(MeshComponent);
		// PointLightComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
	else if (ZenStructVobLight.LightType == ELightType::Spot)
	{
		const auto SpotLightComponent = NewObject<USpotLightComponent>(this);
		// SpotLightComponent->LightColor = ZenStructVobLight.Color;
		// SpotLightComponent->AttenuationRadius = ZenStructVobLight.Range;
		SpotLightComponent->Intensity = 100000.0f;
		SpotLightComponent->LightColor = FColor(255, 0, 0);
		SpotLightComponent->AttenuationRadius = 16384.0f;
		// SpotLightComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
		SpotLightComponent->RegisterComponent();
	}
}