#include "Game/GHWaypoint.h"

#include "Chaos/Core.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AGHWaypoint::AGHWaypoint()
{
	// Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// Root->SetupAttachment(RootComponent);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// Root->SetupAttachment(RootComponent);
	SetRootComponent(Root);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(Root);
	StaticMeshComponent->SetRelativeRotation(FRotator(0, 90, 0));
	
	NameTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameTextRender"));
	NameTextRenderComponent->SetupAttachment(Root);
	NameTextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	NameTextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;

	PosTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PosTextRender"));
	PosTextRenderComponent->SetupAttachment(Root);
	PosTextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	PosTextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;

	DirTextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DirTextRender"));
	DirTextRenderComponent->SetupAttachment(Root);
	DirTextRenderComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	DirTextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0; // 1000 / 60;
}

void AGHWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHWaypoint::BeginPlay");
	
	SetActorTickEnabled(true);
	// SetActorTickInterval(1000 / 60);

	// FMatrix Matrix = FMatrix::Identity;
	// // Matrix.M
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Matrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	// 	"[0][0]" + FString::SanitizeFloat(Matrix.M[0][0]) +
	// 	"[0][1]" + FString::SanitizeFloat(Matrix.M[0][1]) + 
	// 	"[0][2]" + FString::SanitizeFloat(Matrix.M[0][2]) +
	// 	"   [1][0]" + FString::SanitizeFloat(Matrix.M[1][0]) +
	// 	"[1][1]" + FString::SanitizeFloat(Matrix.M[1][1]) +
	// 	"[1][2]" + FString::SanitizeFloat(Matrix.M[1][2]) +
	// 	"   [2][0]" + FString::SanitizeFloat(Matrix.M[2][0]) +
	// 	"[2][1]" + FString::SanitizeFloat(Matrix.M[2][1]) +
	// 	"[2][2]" + FString::SanitizeFloat(Matrix.M[2][2]));
	// auto Rot = UKismetMathLibrary::Matrix_GetRotator(Matrix);
	// //Rot.MakeFromEuler()
	//
	// const FRotator YawRotation(0, this->GetActorRotation().Yaw, 0);
	// const FVector fwd = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// // static TMatrix<T> MakeFromX(TVector<T> const& XAxis);
	//
	// auto RotMatrix = FRotationMatrix(YawRotation);
	//
	// FTimerHandle Handle;
	// GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this]
	// {
	// 	const FRotator YawRotation(0, this->GetActorRotation().Yaw, 0);
	// 	auto RotMatrix = FRotationMatrix(YawRotation);
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, RotMatrix.ToString());
	// }), 1.0f, true);
}

// Called every frame
void AGHWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHWaypoint::Tick");

	auto CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	auto CameraLocation = CameraManager->GetCameraLocation();
	auto WaypointLocation = GetActorLocation();
	if (FVector::Dist(CameraLocation, WaypointLocation) > 10000)
		return;
	
	auto Rotation = UKismetMathLibrary::FindLookAtRotation(WaypointLocation, CameraLocation);
	Rotation.Roll = 0;
	Rotation.Pitch = 0;
	
	NameTextRenderComponent->SetWorldRotation(Rotation);
	PosTextRenderComponent->SetWorldRotation(Rotation);
	DirTextRenderComponent->SetWorldRotation(Rotation);
}