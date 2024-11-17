#include "Game/Vobs/GHVobsLoader.h"


// void GHVobsLoader::LoadVobs(TArray<TSharedPtr<FJsonValue>> JsonVobArray)
// {
// 	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
// 	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
// 	
// 	for (const auto JsonVob : JsonVobArray)
// 	{
// 		const auto JsonVobObject = JsonVob->AsObject();
//
// 		FVobStruct ZenStructVob;
//
// 		// JsonVobObject->TryGetStringField("class", ZenStructVob.Class);
// 		JsonVobObject->TryGetNumberField("pack", ZenStructVob.Pack);
// 		JsonVobObject->TryGetStringField("presetName", ZenStructVob.PresetName);
// 		
// 		const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
// 		JsonVobObject->TryGetArrayField("bbox3DWS", JsonValueArray);
// 		if (JsonValueArray->Num() == 6)
// 		{
// 			int32 i = 0;
// 			for (const auto JsonValue : *JsonValueArray)
// 			{
// 				float OutputFloat;
// 				const auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
// 				if (bIsFloatValid)
// 					ZenStructVob.BBox3DWS[i] = OutputFloat;
// 				
// 				i++;
// 			}
// 		}
// 		
// 		JsonVobObject->TryGetArrayField("trafoOSToWSRot", JsonValueArray);
// 		if (JsonValueArray->Num() == 3)
// 		{
// 			int32 i = 0;
// 			FVector OutputVector;
// 			for (const auto JsonValue : *JsonValueArray)
// 			{
// 				float OutputFloat;
// 				const auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
// 				if (bIsFloatValid)
// 					OutputVector[i] = OutputFloat;
// 				
// 				i++;
// 			}
// 			
// 			ZenStructVob.TrafoOSToWSRot.Roll = OutputVector.X;
// 			ZenStructVob.TrafoOSToWSRot.Pitch = OutputVector.Y;
// 			ZenStructVob.TrafoOSToWSRot.Yaw = OutputVector.Z;
// 		}
// 		
// 		JsonVobObject->TryGetArrayField("trafoOSToWSPos", JsonValueArray);
// 		if (JsonValueArray->Num() == 3)
// 		{
// 			int32 i = 0;
// 			for (const auto JsonValue : *JsonValueArray)
// 			{
// 				float OutputFloat;
// 				const auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
// 				if (bIsFloatValid)
// 					ZenStructVob.TrafoOSToWSPos[i] = OutputFloat;
// 				
// 				i++;
// 			}
// 		}
// 		JsonVobObject->TryGetStringField("vobName", ZenStructVob.VobName);
// 		JsonVobObject->TryGetStringField("visual", ZenStructVob.Visual);
// 		JsonVobObject->TryGetBoolField("showVisual", ZenStructVob.ShowVisual);
// 		JsonVobObject->TryGetNumberField("visualCamAlign", ZenStructVob.VisualCamAlign);
// 		JsonVobObject->TryGetBoolField("cdStatic", ZenStructVob.CdStatic);
// 		JsonVobObject->TryGetBoolField("cdDyn", ZenStructVob.CdDyn);
// 		JsonVobObject->TryGetBoolField("staticVob", ZenStructVob.StaticVob);
// 		JsonVobObject->TryGetNumberField("dynShadow", ZenStructVob.DynShadow);
// 		JsonVobObject->TryGetNumberField("zbias", ZenStructVob.ZBias);
// 		JsonVobObject->TryGetBoolField("isAmbient", ZenStructVob.IsAmbient);
// 		// JsonVobObject->TryGetStringField("itemInstance", ZenStructVob.ItemInstance);
//
// 		UE_LOG(LogTemp, Warning, TEXT("Vob.Class %s Vob.Pack %d Vob.PresetName %s"), *ZenStructVob.Class, ZenStructVob.Pack, *ZenStructVob.PresetName);
//
// 		if (GameMode)
// 			GameMode->VobArray.Add(ZenStructVob);
// 	}
// }


void GHVobsLoader::FillVobStruct(FVobStruct &Struct, TSharedPtr<FJsonObject> JsonObject)
{
	JsonObject->TryGetStringField(ANSI_TO_TCHAR("class"), Struct.Class);
	JsonObject->TryGetNumberField(ANSI_TO_TCHAR("pack"), Struct.Pack);
	JsonObject->TryGetStringField(ANSI_TO_TCHAR("presetName"), Struct.PresetName);
	
	const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
	JsonObject->TryGetArrayField(ANSI_TO_TCHAR("bbox3DWS"), JsonValueArray);
	if (JsonValueArray->Num() == 6)
	{
		int32 i = 0;
		for (const auto JsonValueFloat : *JsonValueArray)
		{
			float OutputFloat;
			const auto bIsFloatValid = JsonValueFloat->TryGetNumber(OutputFloat);
			if (bIsFloatValid)
				Struct.BBox3DWS[i] = OutputFloat;
			
			i++;
		}
	}
	
	JsonObject->TryGetArrayField(ANSI_TO_TCHAR("trafoOSToWSRot"), JsonValueArray);
	if (JsonValueArray->Num() == 3)
	{
		int32 i = 0;
		FVector OutputVector;
		for (const auto JsonValueFloat : *JsonValueArray)
		{
			float OutputFloat;
			const auto bIsFloatValid = JsonValueFloat->TryGetNumber(OutputFloat);
			if (bIsFloatValid)
				OutputVector[i] = OutputFloat;
			
			i++;
		}
		
		Struct.TrafoOSToWSRot.Roll = OutputVector.X;
		Struct.TrafoOSToWSRot.Pitch = OutputVector.Y;
		Struct.TrafoOSToWSRot.Yaw = OutputVector.Z;
	}
	else if (JsonValueArray->Num() == 9)
	{
		TArray<float> MatrixFloat = {};
		
		for (const auto JsonValueFloat : *JsonValueArray)
		{
			float OutputFloat;
			auto bIsFloatValid = JsonValueFloat->TryGetNumber(OutputFloat);
			if (bIsFloatValid)
				MatrixFloat.Add(OutputFloat);
		}

		if (MatrixFloat.Num() == 9)
		{
			auto XAxisVector = FVector(MatrixFloat[8], MatrixFloat[6], MatrixFloat[7]);
			auto YAxisVector = FVector(MatrixFloat[2], MatrixFloat[0], MatrixFloat[1]);
			auto ZAxisVector = FVector(MatrixFloat[5], MatrixFloat[3], MatrixFloat[4]);
			auto RotationMatrix = FBasisVectorMatrix(XAxisVector, YAxisVector, ZAxisVector, FVector::ZeroVector);

			Struct.TrafoOSToWSRot = RotationMatrix.Rotator();
		}
	}
	
	JsonObject->TryGetArrayField(ANSI_TO_TCHAR("trafoOSToWSPos"), JsonValueArray);
	if (JsonValueArray->Num() == 3)
	{
		int32 i = 0;
		for (const auto JsonValueFloat : *JsonValueArray)
		{
			float OutputFloat;
			const auto bIsFloatValid = JsonValueFloat->TryGetNumber(OutputFloat);
			if (bIsFloatValid)
				Struct.TrafoOSToWSPos[i] = OutputFloat;
			
			i++;
		}
	}

	JsonObject->TryGetStringField(ANSI_TO_TCHAR("vobName"), Struct.VobName);
	JsonObject->TryGetStringField(ANSI_TO_TCHAR("visual"), Struct.Visual);
	JsonObject->TryGetBoolField(ANSI_TO_TCHAR("showVisual"), Struct.ShowVisual);
	JsonObject->TryGetNumberField(ANSI_TO_TCHAR("visualCamAlign"), Struct.VisualCamAlign);
	JsonObject->TryGetBoolField(ANSI_TO_TCHAR("cdStatic"), Struct.CdStatic);
	JsonObject->TryGetBoolField(ANSI_TO_TCHAR("cdDyn"), Struct.CdDyn);
	JsonObject->TryGetBoolField(ANSI_TO_TCHAR("staticVob"), Struct.StaticVob);
	JsonObject->TryGetNumberField(ANSI_TO_TCHAR("dynShadow"), Struct.DynShadow);
	JsonObject->TryGetNumberField(ANSI_TO_TCHAR("zbias"), Struct.ZBias);
	JsonObject->TryGetBoolField(ANSI_TO_TCHAR("isAmbient"), Struct.IsAmbient);

	// UE_LOG(LogTemp, Warning, TEXT("Vob.Class %s Vob.Pack %d Vob.PresetName %s"), *Struct.Class, Struct.Pack, *Struct.PresetName);
	// UE_LOG(LogTemp, Warning, TEXT("Vob position: %s"), *Struct.TrafoOSToWSPos.ToString());
}

void GHVobsLoader::FillVobLightStruct(FVobLightStruct &Struct, TSharedPtr<FJsonObject> JsonObject)
{
	FillVobStruct(Struct.VobStruct, JsonObject);
	
	JsonObject->TryGetNumberField(ANSI_TO_TCHAR("range"), Struct.Range);
}

void GHVobsLoader::LoadVobsFile()
{
	const FString VobsFilePath = "C:/archolos/vobs/ARCHOLOS_MAINLAND.json";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*VobsFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("File not found: vobs.json"));
		return;
	}
	
	FString VobsFileData = "";
	if (!FFileHelper::LoadFileToString(VobsFileData, *VobsFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant read file: vobs.json"));
		return;
	}
	
	const auto JsonReader = TJsonReaderFactory<>::Create(VobsFileData);
	TSharedPtr<FJsonObject> JsonRootObject;
	const auto DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	if (!DeserializeStatus || !JsonRootObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant deserialize json data: vobs.json"));
		return;
	}

	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	auto JsonValueArray = JsonRootObject->GetArrayField(ANSI_TO_TCHAR("zCVob"));
	for (const auto JsonValue : JsonValueArray)
	{
		FVobStruct VobStruct;

		FillVobStruct(VobStruct, JsonValue->AsObject());
		
		GameMode->VobArray.Add(VobStruct);
	}

	// JsonValueArray = JsonRootObject->GetArrayField("zCVobLight");
	// for (const auto JsonValue : JsonValueArray)
	// {
	// 	FVobLightStruct VobLightStruct;
	//
	// 	FillVobLightStruct(VobLightStruct, JsonValue->AsObject());
	//
	// 	GameMode->VobLightArray.Add(VobLightStruct);
	// }
	
	// const auto JsonVobArray = JsonRootObject->GetArrayField("zCVob");
	// LoadVobs(JsonVobArray);

	// const auto JsonWaypointArray = JsonRootObject->GetArrayField("zCWaypoint");
	// LoadWaypoints(JsonWaypointArray);
}
