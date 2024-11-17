// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GHJSON.h"


#include "Core/GHAssetManager.h"
#include "Core/GHGameInstance.h"
#include "Misc/DefaultValueHelper.h"
#include "Kismet/GameplayStatics.h"


#include "Game/GHWaypoint.h"



DEFINE_LOG_CATEGORY(JSONParser);

void GHJSON::LoadWaypoints()
{
	FString WaypointFilePath = "C:/archolos/vobs/waypoints.json";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*WaypointFilePath))
	{
		UE_LOG(JSONParser, Warning, TEXT("File not found: waypoints.json"));
		return;
	}
	
	FString WaypointFileData = "";
	if (!FFileHelper::LoadFileToString(WaypointFileData, *WaypointFilePath))
	{
		UE_LOG(JSONParser, Warning, TEXT("Cant read file: waypoints.json"));
		return;
	}
	
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(WaypointFileData);
	TSharedPtr<FJsonObject> JsonRootObject;
	bool DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	if (!DeserializeStatus || !JsonRootObject.IsValid())
	{
		UE_LOG(JSONParser, Warning, TEXT("Cant read json data: waypoints.json"));
		return;
	}
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	TArray<TSharedPtr<FJsonValue>> JsonWaypointArray = JsonRootObject->GetArrayField(ANSI_TO_TCHAR("waypoints"));
	for (auto JsonWaypoint : JsonWaypointArray)
	{
		auto JsonWaypointObject = JsonWaypoint->AsObject();
		
		FWaypoint Waypoint;

		//FJsonObject::TryGetStringField(FStringView FieldName, FString& OutString) const
		JsonWaypointObject->TryGetStringField(ANSI_TO_TCHAR("wpName"), Waypoint.Name);
		JsonWaypointObject->TryGetNumberField(ANSI_TO_TCHAR("waterDepth"), Waypoint.WaterDepth);
		JsonWaypointObject->TryGetBoolField(ANSI_TO_TCHAR("underWater"), Waypoint.UnderWater);

		const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
		auto bIsPositionValid = JsonWaypointObject->TryGetArrayField(ANSI_TO_TCHAR("position"), JsonValueArray);
		if (bIsPositionValid && JsonValueArray->Num() == 3)
		{
			int32 i = 0;
			for (const auto JsonValue : *JsonValueArray)
			{
				float OutputFloat;
				const auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
				if (bIsFloatValid)
					Waypoint.Position[i] = OutputFloat;
				
				i++;
			}
		}

		JsonWaypointObject->TryGetNumberField(ANSI_TO_TCHAR("direction"), Waypoint.Direction.Yaw);
		
		auto bIsLinksValid = JsonWaypointObject->TryGetArrayField(ANSI_TO_TCHAR("links"), JsonValueArray);
		if (bIsLinksValid)
		{
			FString Link = "";
			for (auto JsonValueString : *JsonValueArray)
			{
				auto bIsValueValid = JsonValueString->TryGetString(Link);
				if (bIsValueValid)
					Waypoint.Links.Add(Link);
			}
		}

		if (GameMode)
			GameMode->WaypointMap.Add(Waypoint.Name, Waypoint);
	}
}

void GHJSON::LoadLanguages()
{
	const FString LangFilePath = "C:/gothic_lua_scripts/_test_001/lang_ru_Snowball.json";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*LangFilePath))
	{
		UE_LOG(JSONParser, Warning, TEXT("File not found: waypoints.json"));
		return;
	}
	
	FString LangFileData = "";
	if (!FFileHelper::LoadFileToString(LangFileData, *LangFilePath))
	{
		UE_LOG(JSONParser, Warning, TEXT("Cant read file: waypoints.json"));
		return;
	}
	
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(LangFileData);
	TSharedPtr<FJsonObject> JsonRootObject;
	bool DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	if (!DeserializeStatus || !JsonRootObject.IsValid())
	{
		UE_LOG(JSONParser, Warning, TEXT("Cant read json data: waypoints.json"));
		return;
	}

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	TArray<FString> JsonKeys = {"chapter", "entity", "const", "svm", "log", "letters", "choice", "ai_output"};
	for (auto JsonKey : JsonKeys)
	{
		TMap<FString, TMap<FString, FString>> Section;
		auto JsonSectionArray = JsonRootObject->GetArrayField(JsonKey);
		for (auto JsonSection : JsonSectionArray)
		{
			auto JsonObject = JsonSection->AsObject();
			auto Label = JsonObject->GetStringField(ANSI_TO_TCHAR("Label"));
			auto Text = JsonObject->GetStringField(ANSI_TO_TCHAR("Text"));
	
			TMap<FString, FString> Element;
			Element.Add("Label", Label);
			Element.Add("Text", Text);
			
			Section.Add(Label, Element);
		}
		if (GameMode)
			GameMode->LanguageMap.Add(JsonKey, Section);
	}
	
	// for (auto& Sect : GameMode->LanguageMap)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, Sect.Key);
	// }
		
	// auto JsonItems = JsonRootObject->Values;
	// for (auto JsonItem : JsonItems)
	// {
	// 	auto Key = JsonItem.Key;
	// 	if (["chapter", "entity"])
	// 	// auto Value = JsonItem.Value;
	// 	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, KeyName);
	// }
	//
	// TMap<FString, TMap<FString, TMap<FString, FString>>> LanguageMap;
	//
	// // <TSharedPtr<FJsonValue>> JsonWaypointArray = JsonRootObject->GetArrayField("ai_output");
	// auto JsonWaypointArray = JsonRootObject->GetArrayField("chapter");
	// for (auto JsonWaypoint : JsonWaypointArray)
	// {
	// 	TSharedPtr<FJsonObject> JsonObject = JsonWaypoint->AsObject();
	//
	// 	TMap<FString, FString> Element;
	// 	FString Label = JsonObject->GetStringField("Label");
	// 	FString Text = JsonObject->GetStringField("Text");
	// 	Element.Add("Label", Label);
	// 	Element.Add("Text", Text);
	// 	
	// 	//LanguageMap.Add(Label, Element);
	// }
	//
	// auto El = LanguageMap.Find("Chapter_1");
	// if (El != nullptr)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Element found !!!");
	// 	auto val = El->FindRef("Text");
	// 	 if (!val.IsEmpty())
	// 	 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, val);
	// }
}

FMorphMeshData* GHJSON::ParseMorphMeshData(const FString& FileData)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(FileData);
	TSharedPtr<FJsonObject> JsonRootObject;
	bool DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	if (!DeserializeStatus || !JsonRootObject.IsValid())
	{
		UE_LOG(JSONParser, Warning, TEXT("Can't deserialize json data"));
		return nullptr;
	}

	FString Name;
	JsonRootObject->TryGetStringField(ANSI_TO_TCHAR("name"), Name);
	if (Name.Len() <= 0)
	{
		UE_LOG(JSONParser, Warning, TEXT("Name.Len() <= 0"));
		return nullptr;
	}

	FString Path;
	JsonRootObject->TryGetStringField(ANSI_TO_TCHAR("path"), Path);
	if (Path.Len() <= 0)
	{
		UE_LOG(JSONParser, Warning, TEXT("Path.Len() <= 0"));
		return nullptr;
	}

	auto StructCppOps = FMorphMeshData::StaticStruct()->GetCppStructOps();
	auto MorphGroupData = static_cast<FMorphMeshData*>(FMemory::Malloc(
		StructCppOps->GetSize(), StructCppOps->GetAlignment()));
	StructCppOps->Construct(MorphGroupData);

	MorphGroupData->Name = Name;
	MorphGroupData->Path = Path;

	const TArray<TSharedPtr<FJsonValue>>* JsonAnimationArray;
	JsonRootObject->TryGetArrayField(ANSI_TO_TCHAR("animations"), JsonAnimationArray);
	for (auto JsonAnimation : *JsonAnimationArray)
	{
		auto JsonObject = JsonAnimation->AsObject();

		FMorphAnimData MorphAnimData;
		
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("name"), MorphAnimData.Name);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("layer"), MorphAnimData.Layer);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("blend_id"), MorphAnimData.BlendIn);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("hold"), MorphAnimData.Hold);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("blend_out"), MorphAnimData.BlendOut);
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("flags"), MorphAnimData.Flags);
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("asc_name"), MorphAnimData.ASCName);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("start_frame"), MorphAnimData.FrameStart);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("end_frame"), MorphAnimData.FrameEnd);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("spd"), MorphAnimData.Speed);

		MorphGroupData->MorphAnimDataMap.Add(MorphAnimData.Name, MorphAnimData);

		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Add MorphAnimData: " + MorphAnimData.Name);
	}

	return MorphGroupData;
}

FAnimationGroupData* GHJSON::ParseAnimationGroupData(const FString& FileData)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(FileData);
	TSharedPtr<FJsonObject> JsonRootObject;
	bool DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	if (!DeserializeStatus || !JsonRootObject.IsValid())
	{
		UE_LOG(JSONParser, Warning, TEXT("Can't deserialize json data"));
		return nullptr;
	}

	// @TODO: not used?
	FString GroupName = "";
	JsonRootObject->TryGetStringField(ANSI_TO_TCHAR("group_name"), GroupName);
	if (GroupName.Len() == 0)
	{
		UE_LOG(JSONParser, Warning, TEXT("GroupName.Len() == 0"));
		return nullptr;
	}
	GroupName = GroupName.ToUpper();

	// @TODO: not used?
	FString Skeleton = "";
	JsonRootObject->TryGetStringField(ANSI_TO_TCHAR("skeleton"), Skeleton);
	if (Skeleton.Len() == 0)
	{
		UE_LOG(JSONParser, Warning, TEXT("Skeleton.Len() == 0"));
		return nullptr;
	}
	
	const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
	JsonRootObject->TryGetArrayField(ANSI_TO_TCHAR("supported_mesh"), JsonValueArray);
	if (JsonValueArray->Num() == 0)
	{
		// work with empty supported mesh?
		// UE_LOG(JSONParser, Warning, TEXT("can't parse supported_mesh"));
		// return nullptr;
	}

	auto StructCppOps = FAnimationGroupData::StaticStruct()->GetCppStructOps();
	auto AnimationGroupData = static_cast<FAnimationGroupData*>(FMemory::Malloc(
		StructCppOps->GetSize(), StructCppOps->GetAlignment()));
	StructCppOps->Construct(AnimationGroupData);

	AnimationGroupData->GroupName = GroupName;
	AnimationGroupData->SkeletonFileName = Skeleton;
	
	for (auto JsonValueString : *JsonValueArray)
	{
		FString OutputString;
		auto bIsStringValid = JsonValueString->TryGetString(OutputString);
		if (bIsStringValid)
			AnimationGroupData->SupportedMeshArray.Add(OutputString);
	}

	auto JsonSectionSocketArray = JsonRootObject->GetArrayField(ANSI_TO_TCHAR("sockets"));
	for (auto JsonSocketSection : JsonSectionSocketArray)
	{
		auto JsonSocketObject = JsonSocketSection->AsObject();
		
		FSocketData FSocketData;

		JsonSocketObject->TryGetStringField(ANSI_TO_TCHAR("name"), FSocketData.Name);
		JsonSocketObject->TryGetStringField(ANSI_TO_TCHAR("bone"), FSocketData.Bone);

		const TArray<TSharedPtr<FJsonValue>>* JsonSocketPositionArray;
		auto bIsSocketPositionValid = JsonSocketObject->TryGetArrayField(ANSI_TO_TCHAR("position"), JsonSocketPositionArray);
		if (bIsSocketPositionValid && JsonSocketPositionArray->Num() == 3)
		{
			int32 i = 0;
			for (auto JsonValue : *JsonSocketPositionArray)
			{
				float OutputFloat;
				auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
				if (bIsFloatValid)
					FSocketData.Position[i] = OutputFloat;
				
				i++;
			}
		}

		const TArray<TSharedPtr<FJsonValue>>* JsonSocketRotationArray;
		auto bIsSocketRotationValid = JsonSocketObject->TryGetArrayField(ANSI_TO_TCHAR("rotation"), JsonSocketRotationArray);
		if (bIsSocketRotationValid && JsonSocketRotationArray->Num() == 3)
		{
			int32 i = 0;
			for (auto JsonValue : *JsonSocketRotationArray)
			{
				float OutputFloat;
				auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
				if (bIsFloatValid)
				{
					if (i == 0)
						FSocketData.Rotation.Roll = OutputFloat;
					else if (i == 1)
						FSocketData.Rotation.Pitch = OutputFloat;
					else if (i == 2)
						FSocketData.Rotation.Yaw = OutputFloat;
				}
				
				i++;
			}
		}

		// op [] has no auto add, need null check
		AnimationGroupData->SocketDataMap.Add(FSocketData.Name, FSocketData);
		// UE_LOG(JSONParser, Warning, TEXT("socket: %s, %s, %s"), *FSocketData.Name, *FSocketData.Position.ToString(), *FSocketData.Rotation.ToString());
	}

	auto JsonAnimationSectionArray = JsonRootObject->GetArrayField(ANSI_TO_TCHAR("animations"));
	for (auto JsonAnimationSection : JsonAnimationSectionArray)
	{
		auto JsonObject = JsonAnimationSection->AsObject();
		
		UAnimationData* AnimationData = NewObject<UAnimationData>();
		// Anti GC
		AnimationData->AddToRoot();

		JsonObject->TryGetStringField(ANSI_TO_TCHAR("name"), AnimationData->Name);

		// For convenience / comfortable
		FString NewObjectName = AnimationData->Name + "_" + AnimationData->GetName();
		AnimationData->Rename(*NewObjectName);

		//const TCHAR* newName = *FString(GetNameSafe(this->GetOuter()) + GetNameSafe(this));

		int32 Layer = 1;
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("layer"), Layer);
		AnimationData->Layer = FString::FromInt(Layer);
		
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("next_anim"), AnimationData->NextAnimation);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("blend_id"), AnimationData->BlendIn);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("blend_out"), AnimationData->BlendOut);
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("flags"), AnimationData->Flags);
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("asc_name"), AnimationData->AscName);
		FString Direction = "";
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("direction"), Direction);
		if (Direction.Contains("R"))
			AnimationData->bIsReverse = true;
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("start_time"), AnimationData->StartTime);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("end_time"), AnimationData->EndTime);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("fps"), AnimationData->FPS);

		const TArray<TSharedPtr<FJsonValue>>* JsonEventsArray;
		auto bIsEventsValid = JsonObject->TryGetArrayField(ANSI_TO_TCHAR("events"), JsonEventsArray);
		if (bIsEventsValid && JsonEventsArray->Num() > 0)
		{
			// UE_LOG(JSONParser, Warning, TEXT("anim: %s, events count: %i"), *AnimationData.Name, JsonEventsArray->Num());
			
			for (auto JsonEventsSection : *JsonEventsArray)
			{
				auto JsonEventsSectionObject = JsonEventsSection->AsObject();
			
				FEventData EventData;
			
				JsonEventsSectionObject->TryGetStringField(ANSI_TO_TCHAR("name"), EventData.Name);
				JsonEventsSectionObject->TryGetNumberField(ANSI_TO_TCHAR("time"), EventData.Time);
				if (EventData.Name == "eventTag")
				{
					JsonEventsSectionObject->TryGetStringField(ANSI_TO_TCHAR("tag"), EventData.Tag);
					JsonEventsSectionObject->TryGetStringField(ANSI_TO_TCHAR("arg1"), EventData.Arg1);
				}
				else if (EventData.Name == "eventSFX")
				{
					JsonEventsSectionObject->TryGetStringField(ANSI_TO_TCHAR("effect"), EventData.Effect);
					JsonEventsSectionObject->TryGetNumberField(ANSI_TO_TCHAR("radius"), EventData.Radius);
					JsonEventsSectionObject->TryGetStringField(ANSI_TO_TCHAR("slot"), EventData.Slot);
				}
				else if (EventData.Name == "eventSFXGrnd")
				{
					JsonEventsSectionObject->TryGetStringField(ANSI_TO_TCHAR("effect"), EventData.Effect);
				}
				else if (EventData.Name == "eventSFX")
				{}
				// TODO: add all events
			
				AnimationData->Events.Add(EventData);
			}
		}
		
		// op [] has no auto add, need null check
		// If something already uses the provided key it will be overwritten with the new value.
		AnimationGroupData->AnimationDataMap.Add(AnimationData->Name, AnimationData);
	}

	return AnimationGroupData;
}

TArray<FAudioData*> GHJSON::ParseAudioData(const FString& FileData)
{
	TArray<FAudioData*> AudioDataArray = {};
	
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(FileData);
	TSharedPtr<FJsonObject> JsonRootObject;
	bool DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	if (!DeserializeStatus || !JsonRootObject.IsValid())
	{
		UE_LOG(JSONParser, Warning, TEXT("Can't deserialize json data"));
		return AudioDataArray;
	}

	auto JsonSoundsSectionArray = JsonRootObject->GetArrayField(ANSI_TO_TCHAR("sounds"));
	for (auto JsonSoundsSection : JsonSoundsSectionArray)
	{
		auto StructCppOps = FAudioData::StaticStruct()->GetCppStructOps();
		auto AudioData = static_cast<FAudioData*>(FMemory::Malloc(
			StructCppOps->GetSize(), StructCppOps->GetAlignment()));
		StructCppOps->Construct(AudioData);
		
		auto JsonObject = JsonSoundsSection->AsObject();
		
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("name"), AudioData->Name);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("volume"), AudioData->VolumeCoef);

		FString Categoty;
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("other"), Categoty);
		if (Categoty.Compare("effect", ESearchCase::IgnoreCase) == 0)
			AudioData->Category = ECategory::Effect;
		else if (Categoty.Compare("dialog", ESearchCase::IgnoreCase) == 0)
			AudioData->Category = ECategory::Dialog;
		else if (Categoty.Compare("music", ESearchCase::IgnoreCase) == 0)
			AudioData->Category = ECategory::Music;
		else if (Categoty.Compare("environments", ESearchCase::IgnoreCase) == 0)
			AudioData->Category = ECategory::Environments;
		else
			AudioData->Category = ECategory::Other;
		
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("range"), AudioData->Range);

		const TArray<TSharedPtr<FJsonValue>>* JsonFilesArray;
		auto bIsFilesValid = JsonObject->TryGetArrayField(ANSI_TO_TCHAR("files"), JsonFilesArray);
		if (bIsFilesValid && JsonFilesArray->Num() > 0)
		{
			// UE_LOG(JSONParser, Warning, TEXT("sound: %s, files count: %i"), *AudioData.Name, JsonFilesArray->Num());
			
			for (auto JsonFilesSection : *JsonFilesArray)
			{
				auto JsonFilesSectionObject = JsonFilesSection->AsObject();
			
				FAudioFile AudioFile;
			
				JsonFilesSectionObject->TryGetStringField(ANSI_TO_TCHAR("name"), AudioFile.Name);
				JsonFilesSectionObject->TryGetNumberField(ANSI_TO_TCHAR("volume"), AudioFile.Volume);
				JsonFilesSectionObject->TryGetNumberField(ANSI_TO_TCHAR("weight"), AudioFile.Weight);
			
				AudioData->FileArray.Add(AudioFile);
			}
		}

		AudioDataArray.Add(AudioData);
	}

	return AudioDataArray;
}


void GHJSON::LoadAnimations()
{
// 	// FString AnimFilePath = "C:/archolos/amin_test.json";
// 	FString AnimFilePath = "C:/archolos/HUMANS.json";
// 	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*AnimFilePath))
// 	{
// 		UE_LOG(JSONParser, Warning, TEXT("File not found: amin_test.json"));
// 		return;
// 	}
// 	
// 	FString AnimFileData = "";
// 	if (!FFileHelper::LoadFileToString(AnimFileData, *AnimFilePath))
// 	{
// 		UE_LOG(JSONParser, Warning, TEXT("Cant read file: amin_test.json"));
// 		return;
// 	}
// 	
// 	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(AnimFileData);
// 	TSharedPtr<FJsonObject> JsonRootObject;
// 	bool DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
// 	if (!DeserializeStatus || !JsonRootObject.IsValid())
// 	{
// 		UE_LOG(JSONParser, Warning, TEXT("Cant read json data: amin_test.json"));
// 		return;
// 	}
//
// 	auto World = GEngine->GetCurrentPlayWorld(nullptr);
// 	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
// 	if (!GameInstance)
// 	{
// 		UE_LOG(JSONParser, Warning, TEXT("!GameInstance"));
// 		return;
// 	}
//
// 	FString GroupName = "";
// 	JsonRootObject->TryGetStringField("group_name", GroupName);
// 	if (GroupName.Len() == 0)
// 	{
// 		UE_LOG(JSONParser, Warning, TEXT("GroupName.Len() == 0"));
// 		return;
// 	}
// 	
// 	FString Skeleton = "";
// 	JsonRootObject->TryGetStringField("skeleton", Skeleton);
// 	if (Skeleton.Len() == 0)
// 	{
// 		UE_LOG(JSONParser, Warning, TEXT("Skeleton.Len() == 0"));
// 		return;
// 	}
// 	
// 	// add for [] op, else fatal
// 	if (!GameInstance->SharedAnimationGroupDataMap.Contains(GroupName))
// 		GameInstance->SharedAnimationGroupDataMap.Add(GroupName, {});
// 	
// 	GameInstance->SharedAnimationGroupDataMap[GroupName].GroupName = GroupName;
// 	GameInstance->SharedAnimationGroupDataMap[GroupName].SkeletonFileName = Skeleton;
//
// 	const TArray<TSharedPtr<FJsonValue>>* JsonValueArray;
// 	JsonRootObject->TryGetArrayField("supported_mesh", JsonValueArray);
// 	for (auto JsonValueString : *JsonValueArray)
// 	{
// 		FString OutputString;
// 		auto bIsStringValid = JsonValueString->TryGetString(OutputString);
// 		if (bIsStringValid)
// 			GameInstance->SharedAnimationGroupDataMap[GroupName].SupportedMeshArray.Add(OutputString);
// 	}
// 	if (GameInstance->SharedAnimationGroupDataMap[GroupName].SupportedMeshArray.Num() <= 0)
// 	{
// 		UE_LOG(JSONParser, Warning, TEXT("can't parse supported_mesh"));
// 		return;
// 	}
//
// 	auto JsonSectionSocketArray = JsonRootObject->GetArrayField("sockets");
// 	for (auto JsonSocketSection : JsonSectionSocketArray)
// 	{
// 		auto JsonSocketObject = JsonSocketSection->AsObject();
// 		
// 		FSocketData FSocketData;
//
// 		JsonSocketObject->TryGetStringField("name", FSocketData.Name);
// 		JsonSocketObject->TryGetStringField("bone", FSocketData.Bone);
//
// 		const TArray<TSharedPtr<FJsonValue>>* JsonSocketPositionArray;
// 		auto bIsSocketPositionValid = JsonSocketObject->TryGetArrayField("position", JsonSocketPositionArray);
// 		if (bIsSocketPositionValid && JsonSocketPositionArray->Num() == 3)
// 		{
// 			int32 i = 0;
// 			for (auto JsonValue : *JsonSocketPositionArray)
// 			{
// 				float OutputFloat;
// 				auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
// 				if (bIsFloatValid)
// 					FSocketData.Position[i] = OutputFloat;
// 				
// 				i++;
// 			}
// 		}
//
// 		const TArray<TSharedPtr<FJsonValue>>* JsonSocketRotationArray;
// 		auto bIsSocketRotationValid = JsonSocketObject->TryGetArrayField("rotation", JsonSocketRotationArray);
// 		if (bIsSocketRotationValid && JsonSocketRotationArray->Num() == 3)
// 		{
// 			int32 i = 0;
// 			for (auto JsonValue : *JsonSocketRotationArray)
// 			{
// 				float OutputFloat;
// 				auto bIsFloatValid = JsonValue->TryGetNumber(OutputFloat);
// 				if (bIsFloatValid)
// 				{
// 					if (i == 0)
// 						FSocketData.Rotation.Roll = OutputFloat;
// 					else if (i == 1)
// 						FSocketData.Rotation.Pitch = OutputFloat;
// 					else if (i == 2)
// 						FSocketData.Rotation.Yaw = OutputFloat;
// 				}
// 				
// 				i++;
// 			}
// 		}
//
// 		// op [] has no auto add, need null check
// 		GameInstance->SharedAnimationGroupDataMap[GroupName].SocketDataMap.Add(FSocketData.Name, FSocketData);
// 	}
//
// 	auto JsonAnimationSectionArray = JsonRootObject->GetArrayField("animations");
// 	for (auto JsonAnimationSection : JsonAnimationSectionArray)
// 	{
// 		auto JsonObject = JsonAnimationSection->AsObject();
// 		
// 		FAnimationData AnimationData;
//
// 		JsonObject->TryGetStringField("name", AnimationData.Name);
//
// 		int32 Layer = 1;
// 		JsonObject->TryGetNumberField("layer", Layer);
// 		AnimationData.Layer = FString::FromInt(Layer);
// 		
// 		JsonObject->TryGetStringField("next_anim", AnimationData.NextAnimation);
// 		JsonObject->TryGetNumberField("blend_id", AnimationData.BlendIn);
// 		JsonObject->TryGetNumberField("blend_out", AnimationData.BlendOut);
// 		JsonObject->TryGetStringField("flags", AnimationData.Flags);
// 		JsonObject->TryGetStringField("asc_name", AnimationData.AscName);
// 		FString Direction = "";
// 		JsonObject->TryGetStringField("direction", Direction);
// 		if (Direction.Contains("R"))
// 			AnimationData.bIsReverse = true;
// 		JsonObject->TryGetNumberField("start_time", AnimationData.StartTime);
// 		JsonObject->TryGetNumberField("end_time", AnimationData.EndTime);
// 		JsonObject->TryGetNumberField("fps", AnimationData.FPS);
//
// 		const TArray<TSharedPtr<FJsonValue>>* JsonEventsArray;
// 		auto bIsEventsValid = JsonObject->TryGetArrayField("events", JsonEventsArray);
// 		if (bIsEventsValid && JsonEventsArray->Num() > 0)
// 		{
// 			// UE_LOG(JSONParser, Warning, TEXT("anim: %s, events count: %i"), *AnimationData.Name, JsonEventsArray->Num());
// 			
// 			for (auto JsonEventsSection : *JsonEventsArray)
// 			{
// 				auto JsonEventsSectionObject = JsonEventsSection->AsObject();
// 			
// 				FEventData EventData;
// 			
// 				JsonEventsSectionObject->TryGetStringField("name", EventData.Name);
// 				JsonEventsSectionObject->TryGetNumberField("time", EventData.Time);
// 				if (EventData.Name == "eventTag")
// 				{
// 					JsonEventsSectionObject->TryGetStringField("tag", EventData.Tag);
// 					JsonEventsSectionObject->TryGetStringField("arg1", EventData.Arg1);
// 				}
// 				else if (EventData.Name == "eventSFX")
// 				{
// 					JsonEventsSectionObject->TryGetStringField("effect", EventData.Effect);
// 					JsonEventsSectionObject->TryGetNumberField("radius", EventData.Radius);
// 					JsonEventsSectionObject->TryGetStringField("slot", EventData.Slot);
// 				}
// 				else if (EventData.Name == "eventSFXGrnd")
// 				{
// 					JsonEventsSectionObject->TryGetStringField("effect", EventData.Effect);
// 				}
// 				else if (EventData.Name == "eventSFX")
// 				{}
// 				// TODO: add all events
// 			
// 				AnimationData.Events.Add(EventData);
// 			}
// 		}
// 		
// 		// op [] has no auto add, need null check
// 		GameInstance->SharedAnimationGroupDataMap[GroupName].AnimationDataMap.Add(AnimationData.Name, AnimationData);
// 	}
}

void GHJSON::LoadSounds()
{
	FString SoundsFilePath = "C:/archolos/sounds.json";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SoundsFilePath))
	{
		UE_LOG(JSONParser, Warning, TEXT("File not found: sounds.json"));
		return;
	}
	
	FString SoundsFileData = "";
	if (!FFileHelper::LoadFileToString(SoundsFileData, *SoundsFilePath))
	{
		UE_LOG(JSONParser, Warning, TEXT("Cant read file: sounds.json"));
		return;
	}
	
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(SoundsFileData);
	TSharedPtr<FJsonObject> JsonRootObject;
	bool DeserializeStatus = FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	if (!DeserializeStatus || !JsonRootObject.IsValid())
	{
		UE_LOG(JSONParser, Warning, TEXT("Cant read json data: sounds.json"));
		return;
	}

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameInstance = Cast<UGHGameInstance>(UGameplayStatics::GetGameInstance(World));
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode)
	{
		UE_LOG(JSONParser, Warning, TEXT("!GameMode"));
		return;
	}

	auto JsonSoundsSectionArray = JsonRootObject->GetArrayField(ANSI_TO_TCHAR("sounds"));
	for (auto JsonSoundsSection : JsonSoundsSectionArray)
	{
		auto JsonObject = JsonSoundsSection->AsObject();
		
		FAudioData AudioData;
		FString Categoty;
		
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("name"), AudioData.Name);
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("volume"), AudioData.VolumeCoef);
		
		JsonObject->TryGetStringField(ANSI_TO_TCHAR("other"), Categoty);
		if (Categoty.Contains("effect"))
			AudioData.Category = ECategory::Effect;
		else if (Categoty.Contains("dialog"))
			AudioData.Category = ECategory::Dialog;
		else if (Categoty.Contains("music"))
			AudioData.Category = ECategory::Music;
		else if (Categoty.Contains("environments"))
			AudioData.Category = ECategory::Environments;
		else
			AudioData.Category = ECategory::Other;
		
		JsonObject->TryGetNumberField(ANSI_TO_TCHAR("range"), AudioData.Range);

		const TArray<TSharedPtr<FJsonValue>>* JsonFilesArray;
		auto bIsFilesValid = JsonObject->TryGetArrayField(ANSI_TO_TCHAR("files"), JsonFilesArray);
		if (bIsFilesValid && JsonFilesArray->Num() > 0)
		{
			// UE_LOG(JSONParser, Warning, TEXT("sound: %s, files count: %i"), *AudioData.Name, JsonFilesArray->Num());
			
			for (auto JsonFilesSection : *JsonFilesArray)
			{
				auto JsonFilesSectionObject = JsonFilesSection->AsObject();
			
				FAudioFile AudioFile;
			
				JsonFilesSectionObject->TryGetStringField(ANSI_TO_TCHAR("name"), AudioFile.Name);
				JsonFilesSectionObject->TryGetNumberField(ANSI_TO_TCHAR("volume"), AudioFile.Volume);
				JsonFilesSectionObject->TryGetNumberField(ANSI_TO_TCHAR("weight"), AudioFile.Weight);
			
				AudioData.FileArray.Add(AudioFile);
			}
		}

		// GameMode->AssetManager->AudioDataMap.Add(AudioData.Name, AudioData);
	}
}