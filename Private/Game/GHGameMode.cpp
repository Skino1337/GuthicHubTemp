// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/GHGameMode.h"

#include "Game/GHEntity.h"
#include "Game/GHLevel.h"
#include "Player/GHCharacter.h"
#include "Core/GHLuaState.h"
#include "UObject/ConstructorHelpers.h"
#include "Core/GHJSON.h"
#include "Core/GHGameInstance.h"

// #include "Game/GHDialog.h"
#include "Game/GHAnimInstance.h"

#include "LuaMachine.h"
#include "LuaState.h"
#include "LuaBlueprintFunctionLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
// #include "Game/GHDebug.h"
#include "Core/GHAssetManager.h"
#include "Engine/StaticMeshActor.h"
#include "UI/GHDialogText.h"
#include "UI/Chapter/GHIntroduceChapter.h"
#include "..\..\Public\Animations\GHMorphedSkeletalMeshComponent.h"


AGHGameMode::AGHGameMode()
{
	// // set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	// if (PlayerPawnBPClass.Class != nullptr)
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;

	PlayerControllerClass = AGHPlayerController::StaticClass();

	static ConstructorHelpers::FObjectFinder<UMaterial> WaterMaterialObjectFinder(TEXT("/Game/Materials/M_Water"));
	if (WaterMaterialObjectFinder.Object != nullptr)
		WaterMaterial = WaterMaterialObjectFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> WaterFallMaterialObjectFinder(
		TEXT("/Game/Materials/M_WaterFall"));
	if (WaterFallMaterialObjectFinder.Object != nullptr)
		WaterFallMaterial = WaterMaterialObjectFinder.Object;

	//UClass* State = UAdvancedLuaState::StaticClass(); TSubclassOf<ULuaState> State
	//UAdvancedLuaState* State = NewObject<UAdvancedLuaState>(UAdvancedLuaState);
	//TSubclassOf<ULuaState>* State = NewObject<UAdvancedLuaState>();
	//UAdvancedLuaState* State = NewObject<UAdvancedLuaState>((UObject)GetTransientPackage(), ULuaState);
	//ULuaState State = NewObject<ULuaState>((UObject)GetTransientPackage(), UAdvancedLuaState);
	//UAdvancedLuaState State = NewObject(UAdvancedLuaState());
	//UAdvancedLuaState* State = NewObject(UAdvancedLuaState);
	//UClass* State = NewObject<UAdvancedLuaState>();
	//TSubclassOf<ULuaState>* State = NewObject<UAdvancedLuaState>();
	//UAdvancedLuaState* State = FLuaMachineModule::Get().GetLuaState(UAdvancedLuaState::StaticClass(), World);
	// UGHDialog

	Axis = 0;
	NpcArray = {};
	WaypointMap = {};
	QuestMap = {};

	// ---
	// Game Time
	// ---
	
	GameTimeSpeedMultiplier = 6.0f;
	GameTimeDays = 1;
	GameTimeHours = 10;
	GameTimeMinutes = 0;
	GameTimeSeconds = 0;
	
	// ---
}

void AGHGameMode::BeginPlay()
{
	Super::BeginPlay();

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHGameMode::BeginPlay");
	
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);

	// AssetManager = NewObject<UGHAssetManager>(this);
	// GHJSON::LoadSounds();
	// AssetManager->Initialization();
	// AssetManager->LoadAllAudioInAudioDataMap();

	// LoadAnimatoins();
	
	Chapter = NewObject<UGHChapter>();
	Chapter->Initialization();
	
	Dialog = NewObject<UGHDialog>();
	Dialog->Initialization();

	Inventory = NewObject<UGHInventory>();
	Inventory->Initialization();

	GLTFRuntime = NewObject<UGHGLTFRuntime>();
	
	Stats = NewObject<UGHStats>();
	Stats->Initialization();

	Quest = NewObject<UGHQuest>();
	Quest->Initialization();

	Debug = NewObject<UGHDebug>();
	Debug->Initialization();

	FTimerHandle GameTimeTimer;
	GetWorldTimerManager().SetTimer(GameTimeTimer, this, &AGHGameMode::UpdateGameTime,
		1.0f / GameTimeSpeedMultiplier, true);

	GHVobsLoader::LoadVobsFile();
	
	GHJSON::LoadWaypoints();
	GHJSON::LoadLanguages();


	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return;
	}
	
	// Add Static Mesh Component node

	auto StaticMeshMap = GameInstance->AssetManager->LoadWorld("ARCHOLOS_MAINLAND_SPLIT");
	auto WaterStaticMesh = StaticMeshMap.FindRef("WATER");
	if (WaterStaticMesh)
	{
		FTransform SpawnTransform {
			FRotator{ 0.0f, 90.0f, 0.0f },    // Rotation
			FVector{ 0.0f, 0.0f, 0.0f },        // Translation
			FVector{ 1.0f, 1.0f, 1.0f }         // Scale
		};
		auto WaterStaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnTransform);
		auto StaticMeshComponent = WaterStaticMeshActor->GetStaticMeshComponent();
		if (StaticMeshComponent)
		{
			StaticMeshComponent->SetStaticMesh(WaterStaticMesh);

			if (WaterMaterial)
			{
				for (int32 i = 0; true; i++)
				{
					auto MaterialInterface = StaticMeshComponent->GetMaterial(i);
					if (!MaterialInterface)
						break;

					auto NewMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(WaterMaterial, this);
					if (!NewMaterialInstanceDynamic)
						continue;

					TArray<UTexture*> UsedTextureArray;
					MaterialInterface->GetUsedTextures(UsedTextureArray, EMaterialQualityLevel::Epic,
						true, ERHIFeatureLevel::SM6, true);

					UTexture* BaseColorTexture = nullptr;
					for (auto UsedTexture : UsedTextureArray)
					{
						if (UsedTexture->GetName().Contains("2D"))
							BaseColorTexture = UsedTexture;
					}
					if (!BaseColorTexture)
						continue;
					
					NewMaterialInstanceDynamic->SetTextureParameterValue(FName("Base Texture"), BaseColorTexture);
					StaticMeshComponent->SetMaterial(i, NewMaterialInstanceDynamic);
				}
			}
		}
	}
	auto AlphaStaticMesh = StaticMeshMap.FindRef("ALPHA");
	if (AlphaStaticMesh)
	{
		FTransform SpawnTransform {
			FRotator{ 0.0f, 90.0f, 0.0f },    // Rotation
			FVector{ 0.0f, 0.0f, 0.0f },        // Translation
			FVector{ 1.0f, 1.0f, 1.0f }         // Scale
		};
		auto WaterStaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnTransform);
		auto StaticMeshComponent = WaterStaticMeshActor->GetStaticMeshComponent();
		if (StaticMeshComponent)
		{
			StaticMeshComponent->SetStaticMesh(AlphaStaticMesh);
		}
	}
	
	// ---

	// Dynamic
	// if (LevelClass)
	// {
	// 	FTransform SpawnTransform;
	// 	SpawnTransform.SetLocation(FVector(0.f, 0.0f, -4000.0f));
	// 	auto Level = World->SpawnActor<AGHLevel>(LevelClass, SpawnTransform);
	// 	if (Level)
	// 	{
	// 	}
	// }

	
	FWaypoint Waypoint;
	Waypoint.Name = "TEST_001";
	Waypoint.Position = FVector(-144339.0, -45885.5, -1902.6);
	WaypointMap.Add(Waypoint.Name, Waypoint);
	
	TArray<FString> FilesPath = {};
	FilesPath.Add("_inner.lua");
	FilesPath.Add("constants.lua");
	FilesPath.Add("classes.lua");
	FilesPath.Add("attributes.lua");
	FilesPath.Add("talents.lua");
	FilesPath.Add("items.lua");
	FilesPath.Add("guilds.lua");
	FilesPath.Add("Npc.lua");
	FilesPath.Add("dialogs/DIA_PC_Thief.lua");
	// FilesPath.Add("test_001.lua");
	for (FString FilePath : FilesPath)
	{
		FString FullFilePath = "C:/gothic_lua_scripts/_test_001/" + FilePath;
		ULuaBlueprintFunctionLibrary::LuaRunNonContentFile(World, UGHLuaState::StaticClass(), FullFilePath, false);
	}

	// Spawn Player
	if (NpcClass)
	{
		const auto LuaValuePlayerInstance = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World,
			UGHLuaState::StaticClass(), "PC_Hero");
		if (LuaValuePlayerInstance.Type != ELuaValueType::Table)
		{
			UE_LOG(LogTemp, Warning, TEXT("LuaValuePlayerInstance.Type != ELuaValueType::Table"));
			return;
		}

		auto LuaValueSpawnPoint = ULuaBlueprintFunctionLibrary::LuaCreateTable(World, UGHLuaState::StaticClass());
		// LuaValueSpawnPoint.SetField("x", FLuaValue(900));
		// LuaValueSpawnPoint.SetField("y", FLuaValue(1100));
		// LuaValueSpawnPoint.SetField("z", FLuaValue(92));
		// LuaValueSpawnPoint.SetField("x", FLuaValue(5717.720467f));
		// LuaValueSpawnPoint.SetField("y", FLuaValue(-36364.489662f));
		// LuaValueSpawnPoint.SetField("z", FLuaValue(5397.493644));

		auto LuaValueFunction = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World,
			UGHLuaState::StaticClass(), "Wld_InsertNpc");
		if (LuaValueFunction.Type == ELuaValueType::Nil)
		{
			UE_LOG(LogTemp, Warning, TEXT("LuaValueFunction.Type == ELuaValueType::Nil"));
			return;
		}
		
		auto LuaValuePlayer = ULuaBlueprintFunctionLibrary::LuaGlobalCallValue(World, UGHLuaState::StaticClass(),
			LuaValueFunction, {LuaValuePlayerInstance, FLuaValue("TEST_001")});
		if (LuaValuePlayer.Type != ELuaValueType::UObject)
		{
			UE_LOG(LogTemp, Warning, TEXT("LuaValuePlayer.Type != ELuaValueType::UObject"));
			return;
		}

		// return LuaValueNewInstance;
		// FTransform SpawnTransform;
		// SpawnTransform.SetLocation(FVector(900.f, 1100.f, 92.f));
		// auto Npc = World->SpawnActorDeferred<AGHNPC>(NpcClass, SpawnTransform);
		// if (Npc)
		// {
		// 	auto LuaValuePlayerTable = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, UGHLuaState::StaticClass(), "PC_Hero");
		// 	if (LuaValuePlayerTable.Type == ELuaValueType::Table)
		// 	{
		// 		Npc->_table = LuaValuePlayerTable;
		// 	}
		//
		// 	Npc->FinishSpawning(SpawnTransform);
		// 	NpcArray.Add(Npc);
		//
		// 	auto PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(World, 0));
		// 	if (PlayerController)
		// 	{
		// 		PlayerController->Possess(Npc);
		// 	}
		// }
	}
	
	FString FullFilePath = "C:/gothic_lua_scripts/_test_001/test_001.lua";
	ULuaBlueprintFunctionLibrary::LuaRunNonContentFile(World, UGHLuaState::StaticClass(), FullFilePath, false);

	//ShowWaypoints(true);
	// SpawnVobs();
	// SpawnVobLights();
	
	// FTransform SpawnTransform(FRotator::ZeroRotator, FVector(-144339.0, -45885.5, -1702.65),
	// 	FVector(1,1,1));
	// auto SpawnedVobLight = World->SpawnActorDeferred<AGHVobLight>(AGHVobLight::StaticClass(), SpawnTransform);
	// if (SpawnedVobLight)
	// {
	// 	// SpawnedVobLight->ZenStructVobLight;
	// 	SpawnedVobLight->FinishSpawning(SpawnTransform);
	// }

	
	// auto PlayerCharacter = Cast<AGHNPC>(UGameplayStatics::GetPlayerCharacter(World, 0));
	// if (PlayerCharacter)
	// {
	// 	NpcArray.Add(PlayerCharacter);
	// }

	// TArray<FString> TestFilesPath = {};
	// TestFilesPath.Add("_inner.lua");
	// TestFilesPath.Add("constants.lua");
	// TestFilesPath.Add("classes.lua");
	// TestFilesPath.Add("attributes.lua");
	// TestFilesPath.Add("talents.lua");
	// TestFilesPath.Add("items.lua");
	// TestFilesPath.Add("Npc.lua");
	// TestFilesPath.Add("dialogs/DIA_PC_Thief.lua");
	// // TestFilesPath.Add("test_001.lua");
	// for (FString FilePath : TestFilesPath)
	// {
	// 	FString FullFilePath = "C:/gothic_lua_scripts/_test_001/" + FilePath;
	// 	ULuaBlueprintFunctionLibrary::LuaRunNonContentFile(World, UDCCLuaState::StaticClass(), FullFilePath, false);
	// }
}

void AGHGameMode::UpdateGameTime()
{
	// @TODO: add pause
	
	GameTimeSeconds = GameTimeSeconds + 1;
	
	if (GameTimeSeconds >= 60)
	{
		GameTimeSeconds = 0;
		GameTimeMinutes = GameTimeMinutes + 1;
		if (GameTimeMinutes >= 60)
		{
			GameTimeMinutes = 0;
			GameTimeHours = GameTimeHours + 1;
			if (GameTimeHours >= 24)
			{
				GameTimeHours = 0;
				GameTimeDays = GameTimeDays + 1;
			}
		}
	}
}

void AGHGameMode::ChangeAxis()
{
	Axis++;

	if (Axis > 2)
	{
		Axis = 0;
	}

	if (Axis == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, "Axis: Pitch");
	}
	else if (Axis == 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, "Axis: Yaw");
	}
	else if (Axis == 2)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, "Axis: Roll");
	}
}

void AGHGameMode::Rot(int32 Side)
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);

	int32 i = 0;
	TArray<AActor*> ActorArray;
	UGameplayStatics::GetAllActorsOfClass(World, VobClass, ActorArray);
	for (auto Actor : ActorArray)
	{
		auto Vob = Cast<AGHVob>(Actor);
		if (!Vob)
		{
			continue;
		}

		auto Rotation = Vob->StaticMeshComponent->GetRelativeRotation();
		if (Axis == 0)
		{
			Rotation.Pitch = Rotation.Pitch + (90.0 * Side);
		}
		if (Axis == 1)
		{
			Rotation.Yaw = Rotation.Yaw + (90.0 * Side);
		}
		if (Axis == 2)
		{
			Rotation.Roll = Rotation.Roll + (90.0 * Side);
		}
		
		Vob->StaticMeshComponent->AddRelativeRotation(Rotation);

		if (i == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Rotation.ToString());
		}
		
		i++;
	}
}

void TestRotationMatrixLeftBox()
{
	auto Rotation = FRotator(158.043525, -176.484328, 1.419245);
	
	// FRotator PitchRotation(Rotation.Pitch, 0, 0); 
	// FRotator YawRotation(0, Rotation.Yaw, 0); 
	// FRotator RollRotation(0, 0, Rotation.Roll);
	
	// auto PitchMatrix = FRotationMatrix(PitchRotation);
	// auto YawMatrix = FRotationMatrix(YawRotation);
	// auto RollMatrix = FRotationMatrix(RollRotation);
	
	auto MatrixFromRotation = FRotationMatrix(Rotation);

	auto RotationFromMatrixFromRotation = MatrixFromRotation.Rotator();
	
	//auto TotalMatrix = PitchMatrix * YawMatrix * RollMatrix;
	
	//auto RotationFromMatrix = TotalMatrix.Rotator();

	// left box
	TArray<float> MatrixFloat = {
		-0.9978118538856506, -0.024768013507127762, -0.06130293384194374,			// 0 1 2
		0.0, -0.9271859526634216, 0.3746076226234436,								// 3 4 5
		-0.06611714512109756, 0.37378793954849243, 0.9251576662063599};				// 6 7 8

	// // Vob_LeftStones
	// TArray<float> MatrixFloat = {
	// 	-0.8910960555076599, 0.12626878917217255, 0.43590047955513,					// 0 1 2
	// 	0.1391313523054123, 0.9902735352516174, -0.0024346038699150085,				// 3 4 5
	// 	-0.43196699023246765, 0.05847780406475067, -0.899994432926178};				// 6 7 8

	// // left box
	// TArray<float> MatrixFloat = {
	// 	-0.9964533448219299, 0.0521007739007473, 0.06611613929271698,				// 0 1 2
	// 	0.052224352955818176, 0.9986362457275391, 0.00014316286251414567,			// 3 4 5
	// 	-0.06601845473051071, 0.003595557529479265, -0.9978148937225342};			// 6 7 8

	// // water barel
	// TArray<float> MatrixFloat = {
	// 	-0.6156622767448425, -0.7869322896003723, -0.0412413589656353,				// 0 1 2
	// 	0.013752772472798824, 0.041597988456487656, -0.9990418553352356,			// 3 4 5
	// 	0.7878931164741516, -0.6156395077705383, -0.01478783879429102};				// 6 7 8
	
	// const auto XAxisVector = FVector(1, 2, 3);
	// const auto YAxisVector = FVector(4, 5, 6);
	// const auto ZAxisVector = FVector(7, 8, 9);
	auto XAxisVector = FVector(MatrixFloat[8], MatrixFloat[6], MatrixFloat[7/*1? 3? 5? 7?*/]);
	auto YAxisVector = FVector(MatrixFloat[2], MatrixFloat[0], MatrixFloat[1/*7? 1? 3? +- true*/]);
	auto ZAxisVector = FVector(MatrixFloat[5], MatrixFloat[3/*1? 3?*/], MatrixFloat[4]);
	// const auto XAxisVector = FVector(MatrixFloat[8], MatrixFloat[6], MatrixFloat[7]);
	// const auto YAxisVector = FVector(MatrixFloat[2], MatrixFloat[0], MatrixFloat[1]);
	// const auto ZAxisVector = FVector(MatrixFloat[5], MatrixFloat[3], MatrixFloat[4]);
	auto Origin = FVector::ZeroVector;
	auto TextRotMatrix = FBasisVectorMatrix(XAxisVector, YAxisVector, ZAxisVector, Origin);
	auto RotationFromCustomMatrix = TextRotMatrix.Rotator();

	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "PitchMatrix: " + PitchMatrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "YawMatrix: " + YawMatrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "RollMatrix: " + RollMatrix.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "RotationFromMatrixFromRotation: " + RotationFromMatrixFromRotation.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "MatrFromRotat: " + MatrixFromRotation.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "TotalMatrix: " + TotalMatrix.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "TextRotMatrix: " + TextRotMatrix.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "RotationOrigin: " + Rotation.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "RotationFromMatrix: " + RotationFromMatrix.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "RotatFromMatrx: " + RotationFromCustomMatrix.ToString());
}

void AGHGameMode::SpawnVobs()
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);

	if (!VobClass)
		return;

	// TestRotationMatrixLeftBox();
	
	// auto Rotation = FRotator(22.04367, 3.51459, -178.578);
	// const FRotator PitchRotation(Rotation.Pitch, 0, 0); 
	// const FRotator YawRotation(0, Rotation.Yaw, 0); 
	// const FRotator RollRotation(0, 0, Rotation.Roll);
	// auto PitchMatrix = FRotationMatrix(PitchRotation);
	// auto YawMatrix = FRotationMatrix(YawRotation);
	// auto RollMatrix = FRotationMatrix(RollRotation);
	// auto TotalMatrix = PitchMatrix * YawMatrix * RollMatrix;
	// auto RotationFromMatrix = TotalMatrix.Rotator();
	//
	// // -0.997812 0 -0.0661171 -0.024768 -0.927186 0.373788 -0.0613029 0.374608 0.925158
	// // -0.997812 -0.024768 -0.0613029 0 -0.927186 0.374608 -0.0661171 0.373788 0.925158
	//
	//
	// const TArray<float> MatrixFloat = {-0.997812, 0, -0.0661171, // 0 1 2
	// 	-0.024768,-0.927186, 0.373788, // 3 4 5
	// 	-0.0613029, 0.374608, 0.925158}; // 6 7 8
	// // const auto XAxisVector = FVector(1, 2, 3);
	// // const auto YAxisVector = FVector(4, 5, 6);
	// // const auto ZAxisVector = FVector(7, 8, 9);
	// // const auto XAxisVector = FVector(MatrixFloat[8], MatrixFloat[6], -MatrixFloat[7]);
	// // const auto YAxisVector = FVector(MatrixFloat[2], MatrixFloat[0], -MatrixFloat[1]);
	// // const auto ZAxisVector = FVector(-MatrixFloat[5], -MatrixFloat[3], MatrixFloat[4]);
	//
	// const auto XAxisVector = FVector(MatrixFloat[8], MatrixFloat[6], MatrixFloat[7]);
	// const auto YAxisVector = FVector(MatrixFloat[2], MatrixFloat[0], MatrixFloat[1]);
	// const auto ZAxisVector = FVector(MatrixFloat[5], MatrixFloat[3], MatrixFloat[4]);
	// const auto Origin = FVector::ZeroVector;
	// const FMatrix TextRotMatrix = FBasisVectorMatrix(XAxisVector, YAxisVector, ZAxisVector, Origin);
	// auto RotationFroTestMatrix = TextRotMatrix.Rotator();
	//
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "PitchMatrix: " + PitchMatrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "YawMatrix: " + YawMatrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "RollMatrix: " + RollMatrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "TotalMatrix: " + TotalMatrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "RotationFromMatrix: " + RotationFromMatrix.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 1995.0f, FColor::Yellow, "TextRotMatrix: " + TextRotMatrix.ToString());
	// //const FVector fwd = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//
	// auto Position = FVector(-145465.0, -46662.7, -1809.52);
	// //auto Rotation = FRotator(0, 0, 0);
	// FTransform SpawnTransform(RotationFroTestMatrix, Position, FVector(1,1,1));
	// auto W = World->SpawnActor<AGHVob>(VobClass, SpawnTransform);
	//GLTFRuntime->LoadModel(W->StaticMeshComponent, "NW_HARBOUR_CRATEBROKEN_01", false);
	//W->StaticMeshComponent->SetRelativeRotation(FRotator(90, 0, 0));

	// return;
	
	for (auto Vob : VobArray)
	{
		// Vob.Visual.Contains("MIN_LOB_COUPLESTONES") || Vob.Visual.Contains("BARREL") || Vob.TrafoOSToWSPos.X == -142942.0 // -144633.0
		if (true)
		{
			FActorSpawnParameters SpawnInfo;
			FTransform SpawnTransform(Vob.TrafoOSToWSRot, Vob.TrafoOSToWSPos,FVector(1,1,1));
			auto SpawnedVob = World->SpawnActorDeferred<AGHVob>(VobClass, SpawnTransform);
			if (SpawnedVob)
			{
				SpawnedVob->ZenStructVob = Vob;
				SpawnedVob->FinishSpawning(SpawnTransform);
	
				// FString Name, Format;
				// Vob.Visual.Split(".",&Name, &Format);
				// GLTFRuntime->LoadModel(SpawnedVob->StaticMeshComponent, Name, false);
				// SpawnedVob->StaticMeshComponent->SetRelativeRotation(FRotator(90, 0, 0));
			}

			//
			// int32 i = 0;
			// for (auto Rot : Vob.RotArray)
			// {
			// 	// if (i != 1)
			// 	// {
			// 	// 	i++;
			// 	// 	continue;
			// 	// }
			// 	
			// 	FVector Pos = Vob.TrafoOSToWSPos;
			// 	Pos.Z = Vob.TrafoOSToWSPos.Z + (120 * i);
			// 	
			// 	FActorSpawnParameters SpawnInfo;
			// 	FTransform SpawnTransform(Rot, Pos,FVector(1,1,1));
			// 	auto SpawnedVob = World->SpawnActorDeferred<AGHVob>(VobClass, SpawnTransform);
			// 	if (SpawnedVob)
			// 	{
			// 		SpawnedVob->ZenStructVob = Vob;
			// 		SpawnedVob->FinishSpawning(SpawnTransform);
			//
			// 		// FString Name, Format;
			// 		// Vob.Visual.Split(".",&Name, &Format);
			// 		// GLTFRuntime->LoadModel(SpawnedVob->StaticMeshComponent, Name, false);
			// 		// SpawnedVob->StaticMeshComponent->SetRelativeRotation(FRotator(90, 0, 0));
			// 	}
			//
			// 	i++;
			// }
	
	
			// FTransform SpawnTransformWP(FRotator::ZeroRotator,
			// 	Vob.TrafoOSToWSPos + FVector(0,0, 100),FVector(1,1,1));
			// auto SpawnedWaypoint = World->SpawnActor<AGHWaypoint>(WaypointClass, SpawnTransformWP);
			// if (SpawnedWaypoint)
			// {
			// 	SpawnedWaypoint->NameTextRenderComponent->SetText(FText::FromString(Vob.Visual));
			// 	
			// 	auto PosText = "X: " + FString::FromInt(Vob.TrafoOSToWSPos.X);
			// 	PosText += ", Y: " + FString::FromInt(Vob.TrafoOSToWSPos.Y);
			// 	PosText += ", Z: " + FString::FromInt(Vob.TrafoOSToWSPos.Z);
			// 	SpawnedWaypoint->PosTextRenderComponent->SetText(FText::FromString(PosText));
			//
			// 	auto DirText = "Roll: " + FString::SanitizeFloat(Vob.TrafoOSToWSRot.Roll);
			// 	DirText += ", Pitch: " + FString::SanitizeFloat(Vob.TrafoOSToWSRot.Pitch);
			// 	DirText += ", Yaw: " + FString::SanitizeFloat(Vob.TrafoOSToWSRot.Yaw);
			// 	SpawnedWaypoint->DirTextRenderComponent->SetText(FText::FromString(DirText));
			//
			// 	if (Vob.ShowVisual && Vob.Visual.Len() > 0)
			// 	{
			// 		FString Name, Format;
			// 		Vob.Visual.Split(".",&Name, &Format);
			//
			// 		if (Format.Contains("3DS"))
			// 		{
			// 			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "model before: " + ZenStructVob.Visual);
			// 			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "model after: " + Name);
			//
			// 			// auto StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
			// 			// StaticMeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
			// 			// StaticMeshComponent->RegisterComponent();
			//
			// 			GLTFRuntime->LoadModel(SpawnedWaypoint->StaticMeshComponent, Name, false);
			// 		}
			// 	}
			// }
		}
	}
}

void AGHGameMode::SpawnVobLights()
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	
	for (auto VobLight : VobLightArray)
	{
		FActorSpawnParameters SpawnInfo;
		FTransform SpawnTransform(VobLight.VobStruct.TrafoOSToWSRot,
			VobLight.VobStruct.TrafoOSToWSPos,FVector(1,1,1));
		auto SpawnedVobLight = World->SpawnActorDeferred<AGHVobLight>(AGHVobLight::StaticClass(), SpawnTransform);
		if (SpawnedVobLight)
		{
			SpawnedVobLight->ZenStructVobLight = VobLight;
			SpawnedVobLight->FinishSpawning(SpawnTransform);
		}
	
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Spawn Vob pos: " + VobLight.VobStruct.TrafoOSToWSPos.ToString());
	}
}

void AGHGameMode::ShowWaypoints(bool bIsShow)
{
	auto World = GEngine->GetCurrentPlayWorld(nullptr);

	if (WaypointClass)
	{
		for (auto Waypoint : WaypointMap)
		{
			FActorSpawnParameters SpawnInfo;
			FTransform SpawnTransform(Waypoint.Value.Direction, Waypoint.Value.Position,FVector(1,1,1));
			auto SpawnedWaypoint = World->SpawnActor<AGHWaypoint>(WaypointClass, SpawnTransform);
			if (SpawnedWaypoint)
			{
				if (Waypoint.Value.Direction.Roll == 0 && Waypoint.Value.Direction.Pitch == 0 && Waypoint.Value.Direction.Yaw == 0)
				{
					SpawnedWaypoint->StaticMeshComponent->AddLocalRotation(FRotator(0, 0, -90));
				}
				
				SpawnedWaypoint->NameTextRenderComponent->SetText(FText::FromString(Waypoint.Value.Name));
			
				auto PosText = "X: " + FString::FromInt(Waypoint.Value.Position.X);
				PosText += ", Y: " + FString::FromInt(Waypoint.Value.Position.Y);
				PosText += ", Z: " + FString::FromInt(Waypoint.Value.Position.Z);
				SpawnedWaypoint->PosTextRenderComponent->SetText(FText::FromString(PosText));

				auto DirText = "Roll: " + FString::SanitizeFloat(Waypoint.Value.Direction.Roll);
				DirText += ", Pitch: " + FString::SanitizeFloat(Waypoint.Value.Direction.Pitch);
				DirText += ", Yaw: " + FString::SanitizeFloat(Waypoint.Value.Direction.Yaw);
				SpawnedWaypoint->DirTextRenderComponent->SetText(FText::FromString(DirText));
			}
		}
	}
}

FString AGHGameMode::GetTranslatedString(FString Data)
{
	if (Data.Len() == 0)
		return "ERROR STRING";
	
	TArray<FString> DataArray;
	auto PartsNum = Data.ParseIntoArray(DataArray,TEXT("."),true);
	if (PartsNum != 2)
		return Data;
	
	auto Section = LanguageMap.Find(DataArray[0]);
	if (Section == nullptr)
		return Data;

	auto Element = Section->Find(DataArray[1]);
	if (Element == nullptr)
		return Data;

	auto Value = Element->FindRef("Text");
	if (Value.IsEmpty())
		return Data;

	return Value;
}

void AGHGameMode::LoadAnimatoins()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AGHGameMode::LoadAnimatoins");


	// auto SkeletalMeshBow = UGHGLTFRuntime::LoadMorphedSkeletalMesh("bow_mm_test_001");
	// if (SkeletalMeshBow)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SkeletalMeshBow OK");
	// 	FActorSpawnParameters SpawnInfo;
	// 	FTransform SpawnTransform(FRotator::ZeroRotator,
	// 		FVector(-144306.0f, -46172.0f, -1840.0f),
	// 		FVector(1,1,1));
	//
	// 	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// 	auto SpawnedBow = World->SpawnActor<AActor>(BowActor, SpawnTransform);
	// 	if (SpawnedBow)
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SpawnedBow OK");
	// 		auto SkeletalMeshComponent = SpawnedBow->FindComponentByClass<UMorphedSkeletalMeshComponent>();
	// 		if (SkeletalMeshComponent)
	// 		{
	// 			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SkeletalMeshComponent OK");
	// 			SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshBow);
	//
	// 			SkeletalMeshComponent->PlayAnimation();
	// 			
	// 			// SkeletalMeshComponent->SetMorphTarget("Key 7", 1.0f);
	// 		}
	// 	}
	// 	// SkeletalMeshComponent->SetSkeletalMesh(LoadedSkeletalMesh);
	// }

	//if (!AssetManager)
	//{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "AssetManager NULL");	
	//}
	//
	// if (AssetManager && AssetManager->SharedMorphDataMap.Num() >= 3 && AssetManager->SharedMorphDataMap["Night of the Raven"].Num() > 0)
	// {
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SharedMorphDataMap.Num() OK");
	// 	
	// 	auto MorphData = AssetManager->GetMorphedSkeletalMesh("Itrw_Bow_H_01");
	// 	if (MorphData.IsValid())
	// 	{
	// 		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "MorphData OK");
	// 		
	// 		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "MorphData.RelativePath: " + MorphData.RelativePath);
	//
	// 		FActorSpawnParameters SpawnInfo;
	// 		FTransform SpawnTransform(FRotator::ZeroRotator,
	// 			FVector(-144306.0f, -46172.0f, -1840.0f),
	// 			FVector(1,1,1));
	//
	// 		auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// 		auto SpawnedBow = World->SpawnActor<AActor>(BowActor, SpawnTransform);
	// 		if (SpawnedBow)
	// 		{
	// 			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SpawnedBow OK");
	// 			auto SkeletalMeshComponent = SpawnedBow->FindComponentByClass<UGHMorphedSkeletalMeshComponent>();
	// 			if (SkeletalMeshComponent)
	// 			{
	// 				// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "SkeletalMeshComponent OK");
	// 				SkeletalMeshComponent->SetSkeletalMesh(MorphData.SkeletalMesh);
	// 				SkeletalMeshComponent->MorphData = MorphData;
	//
	// 				// SkeletalMeshComponent->PlayAnimationByName("S_READY");
	// 				SkeletalMeshComponent->PlayAnimationByName("S_SHOOT");
	// 			
	// 				// SkeletalMeshComponent->SetMorphTarget("Key 7", 1.0f);
	// 			}
	// 		}
	// 	}
	// }

	// TMap<FString, FAnimationData> AnimationDataMap;
	TArray<FString> TestedAnimArray = {
		// RUN
		"S_RUN", "T_RUN_2_RUNL", "S_RUNL", "T_RUNL_2_RUN", "T_JUMPB", "T_RUNSTRAFEL", "T_RUNSTRAFER",
		// WALK
		"S_WALK", "T_WALK_2_WALKL", "S_WALKL", "T_WALKL_2_WALK", "S_WALKBL", "T_WALKSTRAFEL", "T_WALKSTRAFER",
		"T_WALK_2_WALKBL", "T_WALKBL_2_WALK", "T_WALKBR_2_WALK",

		"T_GOTHIT",
		
		//"C_BOW_1",

		"C_BOW_1", "C_BOW_2", "C_BOW_3", "C_BOW_4", "C_BOW_5", "C_BOW_6", "C_BOW_7", "C_BOW_8", "C_BOW_9",

		// crush with this anim
		// one frame bbug?

		// BOW
		"T_RUN_2_BOW", "S_BOW", "T_BOW_2_BOWRUN", "S_BOWRUN", "T_BOWMOVE_2_MOVE", "T_MOVE_2_BOWMOVE", 
		
		// 1H
		"T_RUN_2_1H", "S_1H", "T_1H_2_1HRUN", "S_1HRUN", "T_1HMOVE_2_MOVE", "T_MOVE_2_1HMOVE"};
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
		return;
	}

	GHJSON::LoadAnimations();

	// if (GameInstance->SharedAnimationGroupDataMap.Num() > 0)
	// {
	// 	auto AnimationGroupData = GameInstance->SharedAnimationGroupDataMap.Find("HUMANS");
	// 	if (AnimationGroupData && AnimationGroupData->AnimationDataMap.Num() > 0)
	// 	{
	// 		auto AnimationData = AnimationGroupData->AnimationDataMap.Find("S_RUN");
	// 		if (AnimationData)
	// 		{
	// 			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Found anim asc: " + AnimationData->AscName);
	// 		}
	// 	}
	// }
	//
	// for (auto SharedAnimationGroupDataTuple : GameInstance->SharedAnimationGroupDataMap)
	// {
	// 	auto SharedAnimationGroupData =
	// 		GameInstance->SharedAnimationGroupDataMap.Find(SharedAnimationGroupDataTuple.Key);
	// 	if (!SharedAnimationGroupData)
	// 		continue;
	//
	// 	TMap<int32, FString> BoneNameMap = {};
	// 	
	// 	// load shared skeleton for animation group
	// 	if (!SharedAnimationGroupData->SharedSkeleton)
	// 	{
	// 		auto SkeletalMesh = UGHGLTFRuntime::LoadSkeletalMesh(SharedAnimationGroupData->SkeletonFileName,
	// 			nullptr);
	// 		if (!SkeletalMesh)
	// 			continue;
	// 		
	// 		AGHNPC::SetupSkeletonSockets(SkeletalMesh, SharedAnimationGroupData->SocketDataMap);
	// 		SharedAnimationGroupData->SkeletalMesh = SkeletalMesh;
	// 		
	// 		for (int32 BoneIndex = 0; BoneIndex < SkeletalMesh->GetRefSkeleton().GetNum(); BoneIndex++)
	// 			BoneNameMap.Add(BoneIndex, SkeletalMesh->GetRefSkeleton().GetBoneName(BoneIndex).ToString());
	// 		
	// 		auto Skeleton = SharedAnimationGroupData->SkeletalMesh->GetSkeleton();
	// 		if (!Skeleton)
	// 			continue;
	// 		
	// 		UGHAnimInstance::SetupSkeletonSlots(Skeleton);
	// 		SharedAnimationGroupData->SharedSkeleton = Skeleton;
	//
	// 		// Skeleton->AddSlotGroupName(FName("DefaultGroup"));
	// 		// Skeleton->AddSlotGroupName(FName("2"));
	// 		
	// 		// Skeleton->RegisterSlotNode(FName("DefaultSlot"));
	// 		// Skeleton->RegisterSlotNode(FName("2"));
	// 		// SetSlotGroupName(InSlotName, FAnimSlotGroup::DefaultGroupName)
	// 		
	// 		// Skeleton->SetSlotGroupName(FName("DefaultSlot"), FName("DefaultGroup"));
	// 		// Skeleton->SetSlotGroupName(FName("2"), FName("2"));
	//
	// 		// Skeleton->AddSlotGroupName(FName("G1"));
	// 		// if (Skeleton->FindAnimSlotGroup(FName("G1")))
	// 		// {
	// 		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton have Group: G1");
	// 		// 	Skeleton->SetSlotGroupName(FName("S1"), FName("G1"));
	// 		// 	if (Skeleton->ContainsSlotName(FName("S1")))
	// 		// 	{
	// 		// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton have Slot: S1");
	// 		// 	}
	// 		// }
	//
	// 		// if (Skeleton->ContainsSlotName(FName("DefaultSlot")))
	// 		// {
	// 		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton have DefaultSlot");
	// 		// }
	// 		//
	// 		// if (Skeleton->ContainsSlotName(FName("2")))
	// 		// {
	// 		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Skeleton have 2");
	// 		// }
	// 	}
	//
	// 	for (auto AnimationDataTuple : SharedAnimationGroupData->AnimationDataMap)
	// 	{
	// 		auto AnimationData = SharedAnimationGroupData->AnimationDataMap.Find(AnimationDataTuple.Key);
	//
	// 		// load only tested {"S_RUN", "S_RUNL"};
	// 		if (!TestedAnimArray.Contains(AnimationData->Name))
	// 			continue;
	//
	// 		// if (AnimationData->Name == "C_BOW_1")
	// 		// {
	// 		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "C_BOW_1");
	// 		// }
	// 		
	// 		/** If this is on, it will allow extracting root motion rotation. DEPRECATED in 4.5 root motion is controlled by anim sequences **/
	// 		// bool bEnableRootMotionRotation = false; // cant find
	//
	// 		UAnimSequence* AnimSequence = nullptr;
	// 		AnimSequence = UGHGLTFRuntime::LoadAnimSequence(SharedAnimationGroupData->SkeletalMesh,
	// 							AnimationData->Name);
	// 		
	// 		// if (AnimationData->Name == "C_BOW_3")
	// 		// {
	// 		// 	AnimSequence = UGHGLTFRuntime::LoadAnimSequenceForBlendSpace(SharedAnimationGroupData->SkeletalMesh,
	// 		// 		AnimationData->Name);
	// 		// }
	// 		// else
	// 		// {
	// 		// 	AnimSequence = UGHGLTFRuntime::LoadAnimSequence(SharedAnimationGroupData->SkeletalMesh,
	// 		// 		AnimationData->Name);
	// 		// }
	// 		if (!AnimSequence)
	// 			continue;
	//
	// 		if (AnimationData->bIsReverse)
	// 		{
	// 			AnimSequence->RateScale = -1.0f;
	//
	// 			// fix all events
	// 			auto AnimSequenceLength = AnimSequence->GetPlayLength();
	// 			for (int32 i = 0; i < AnimationData->Events.Num(); i++)
	// 			{
	// 				auto EventTimeReverse = AnimSequenceLength - AnimationData->Events[i].Time;
	// 				if (EventTimeReverse > 0)
	// 				{
	// 					AnimationData->Events[i].Time = EventTimeReverse;
	// 				}
	// 			}
	// 		}
	//
	// 		// get transformed bones
	// 		auto SkeletalMesh = SharedAnimationGroupData->SkeletalMesh;
	// 		if (SkeletalMesh && AnimationData->Layer != "1")
	// 		{
	// 			for (int32 i = 0; i < BoneNameMap.Num(); i++)
	// 			{
	// 				FTransform PrevBoneTransform;
	// 				FTransform CurrBoneTransform;
	// 				FSkeletonPoseBoneIndex BoneIndex = FSkeletonPoseBoneIndex(i);
	// 				auto AnimSequencePlayLength = AnimSequence->GetPlayLength();
	// 				for (double Time = 0.0; Time < AnimSequencePlayLength; Time += 0.1) // 10 fps
	// 				{
	// 					AnimSequence->GetBoneTransform(CurrBoneTransform, BoneIndex, Time, true);
	//
	// 					if (Time == 0.0)
	// 					{
	// 						PrevBoneTransform = CurrBoneTransform;
	// 						continue;
	// 					}
	// 					
	// 					if (!PrevBoneTransform.Equals(CurrBoneTransform, 0.001))
	// 					{
	// 						PrevBoneTransform = CurrBoneTransform;
	// 						AnimationData->BlendBones.Add(BoneNameMap[i]);
	// 						break;
	// 					}
	// 					else
	// 					{
	// 						PrevBoneTransform = CurrBoneTransform;
	// 					}
	// 				}
	// 			}
	//
	// 			// if (AnimationData->Name == "T_MOVE_2_1HMOVE")
	// 			// {
	// 			// 	for (auto TransformedBone : AnimationData->BlendBones)
	// 			// 	{
	// 			// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	// 			// 			"TransformedBone: " + TransformedBone);
	// 			// 	}
	// 			// }
	// 		}
	// 		
	// 		// AnimSequence->bone
	//
	// 		// if (AnimationData->Name == "T_1H_2_1HRUN")
	// 		// {
	// 		// 	UGHAnimInstance::AddAnimNotifyToAnimSequence(AnimSequence);
	// 		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "AddAnimNotifyToAnimSequence");
	// 		// }
	//
	// 		AnimationData->AnimSequence = AnimSequence;
	//
	// 		// auto AnimMontage = UGHAnimInstance::CreateAnimMontageFromAnimSequence(AnimSequence, *AnimationData);
	// 		// if (!AnimMontage)
	// 		// 	continue;
	// 		//
	// 		// AnimMontage->Rename(*(AnimationData->Name + "_ROOT_MOTION"));
	// 		//
	// 		// AnimationData->AnimMontage = AnimMontage;
	//
	// 		FString LogText = "Found anim: " + SharedAnimationGroupData->GroupName + "::" + AnimationData->Name;
	// 		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LogText);
	// 		UE_LOG(LogTemp, Warning, TEXT("%s"), *LogText);
	// 	}
	// }
	//
	// // //auto GLTFRuntime = NewObject<UGHGLTFRuntime>();
	// // for (auto SharedAnimationGroupDataTuple : GameInstance->SharedAnimationGroupDataMap)
	// // {
	// // 	auto GroupName = SharedAnimationGroupDataTuple.Key;
	// // 	auto AnimationDataMap = SharedAnimationGroupDataTuple.Value.AnimationDataMap;
	// // 	UE_LOG(LogTemp, Log, TEXT("LoadAnimatoins: GroupName: %s"), *GroupName);
	// // 	
	// // 	SharedAnimationGroupDataTuple.Value.SkeletalMesh =
	// // 		GLTFRuntime->LoadSkeletalMesh(SharedAnimationGroupDataTuple.Value.Skeleton);
	// // 	
	// // 	for (auto AnimationDataTuple : AnimationDataMap)
	// // 	{
	// // 		auto AnimationName = AnimationDataTuple.Key;
	// // 		auto AnimationData = AnimationDataTuple.Value;
	// // 		UE_LOG(LogTemp, Log, TEXT("LoadAnimatoins: AnimationName: %s"), *AnimationName);
	// //
	// // 		AnimationData.AnimMontage =
	// // 			GLTFRuntime->LoadAnimMontage(SharedAnimationGroupDataTuple.Value.SkeletalMesh, AnimationData.AscName);
	// //
	// // 		if (AnimationData.AnimMontage)
	// // 		{
	// // 			UGHAnimInstance::AnimMontageAddSection(AnimationData.AnimMontage,
	// // 				FName(*AnimationData.Name), AnimationData.StartTime);
	// // 		}
	// // 		//UAnimMontage::AnimMontageAddSection(AnimationData.AnimMontage, AnimationData.Name, AnimationData.StartTime);
	// // 		
	// // 		// AnimMontageAddSection(AnimMontage, TEXT("S_1H"), 0.13f - 0.001f);
	// // 		// AnimMontageAddSection(AnimMontage, TEXT("T_1H_2_1HRUN"), 0.131f + 0.001f);
	// // 		// //Montage_SetPlayRate(AnimMontage, 0.1);
	// // 		// auto MontageLenght = Montage_Play(AnimMontage, -1);
	// // 		// //Montage_SetPlayRate(AnimMontage, 0.1);
	// // 		// Montage_JumpToSection(FName("T_1H_2_1HRUN"), AnimMontage);
	// // 		// //auto Montage = PlaySlotAnimationAsDynamicMontage(AnimationTest, "DefaultSlot", 0.25, 0.25, 1, 1, -1, 0);
	// // 	}
	// // }
	//
	// // now work(((
	// // SpawnAllAnim();
}

void AGHGameMode::SpawnAllAnim()
{
	// auto World = GEngine->GetCurrentPlayWorld(nullptr);
	// auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	// if (!GameInstance)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("!GameInstance"));
	// 	return;
	// }
	//
	// for (auto SharedAnimationGroupDataTuple : GameInstance->SharedAnimationGroupDataMap)
	// {
	// 	auto SharedAnimationGroupData =
	// 		GameInstance->SharedAnimationGroupDataMap.Find(SharedAnimationGroupDataTuple.Key);
	// 	if (!SharedAnimationGroupData)
	// 		continue;
	//
	// 	for (auto AnimationDataTuple : SharedAnimationGroupData->AnimationDataMap)
	// 	{
	// 		auto AnimationData = SharedAnimationGroupData->AnimationDataMap.Find(AnimationDataTuple.Key);
	//
	// 		FTransform SpawnTransform;
	// 		SpawnTransform.SetLocation(FVector(-149180.f, -40680.f, -1960.f));
	// 		auto Actor = World->SpawnActor<AActor>(AActor::StaticClass(), SpawnTransform);
	// 		if (Actor)
	// 		{
	// 			auto ActorComponent = Actor->AddComponentByClass(USkeletalMeshComponent::StaticClass(),
	// 				true, FTransform::Identity, false);
	// 			auto SkeletalMeshComponent  = Cast<USkeletalMeshComponent>(ActorComponent);
	// 			if (SkeletalMeshComponent)
	// 			{
	// 				SkeletalMeshComponent->SetSkeletalMesh(SharedAnimationGroupData->SkeletalMesh);
	// 				SkeletalMeshComponent->PlayAnimation(AnimationData->AnimMontage, true);
	// 				
	// 				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "comp ok");
	// 			}
	// 			//Actor->AddComponent(FName TemplateName, bool bManualAttachment, const FTransform& RelativeTransform, const UObject* ComponentTemplateContext, bool bDeferredFinish)
	// 			// USkeletalMeshComponent* mesh = NewObject<USkeletalMeshComponent>(Actor,"skeletal mesh");
	// 			// mesh->SetupAttachment(Actor->GetRootComponent());
	// 			// mesh->RegisterComponent();
	// 			//
	// 			// mesh->SetSkeletalMesh(SharedAnimationGroupData->SkeletalMesh);
	// 		}
	// 	}
	// }
}