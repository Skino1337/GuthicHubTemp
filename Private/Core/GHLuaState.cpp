// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GHLuaState.h"

#include "Core/GHScript.h"
#include "Core/Helpers.h"

#include "LuaBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Game/GHEntity.h"
#include "Game/GHNPC.h"
#include "Game/GHGameMode.h"


DEFINE_LOG_CATEGORY(Lua);

// 
// print(diego.TestProperty) получает значение или из _table или уже из класса
// diego.protection[0] = 1337 работает через хуки
// print(diego.protection[0]) работает через хуки
//

UGHLuaState::UGHLuaState()
{
	// allow to call native UFunctions with implicit FLuaValue conversions
	bRawLuaFunctionCall = true;
	bLuaOpenLibs = true;
	
	Table.Add("ConcatStrings", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, ConcatStrings)));
	Table.Add("IntToString", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, IntToString)));
	Table.Add("FloatToString", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, FloatToString)));
	Table.Add("Hlp_Random", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_Random)));
	Table.Add("Hlp_StrCmp", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_StrCmp)));
	Table.Add("Hlp_IsValidNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_IsValidNpc)));
	Table.Add("Hlp_IsValidItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_IsValidItem)));
	Table.Add("Hlp_IsItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_IsItem)));
	Table.Add("Hlp_GetNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_GetNpc)));
	Table.Add("Hlp_GetInstanceID", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_GetInstanceID)));
	Table.Add("Hlp_GetInstanceID", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Hlp_GetInstanceID)));
	Table.Add("Npc_GetStateTime", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetStateTime)));
	Table.Add("Npc_SetStateTime", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetStateTime)));
	Table.Add("Npc_GetNearestWP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetNearestWP)));
	Table.Add("Npc_GetNextWP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetNextWP)));
	Table.Add("Npc_GetBodyState", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetBodyState)));
	Table.Add("Npc_HasBodyFlag", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_HasBodyFlag)));
	Table.Add("Npc_SetToFistMode", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetToFistMode)));
	Table.Add("Npc_SetToFightMode", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetToFightMode)));
	Table.Add("Npc_IsInFightMode", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsInFightMode)));
	Table.Add("Npc_GetReadiedWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetReadiedWeapon)));
	Table.Add("Npc_IsDrawingWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsDrawingWeapon)));
	Table.Add("Npc_HasReadiedMeleeWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_HasReadiedMeleeWeapon)));
	Table.Add("Npc_GetTarget", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetTarget)));
	Table.Add("Npc_GetNextTarget", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetNextTarget)));
	Table.Add("Npc_SetTarget", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetTarget)));
	Table.Add("Npc_IsOnFP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsOnFP)));
	Table.Add("Npc_GetInvItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetInvItem)));
	Table.Add("Npc_HasItems", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_HasItems)));
	Table.Add("Npc_RemoveInvItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_RemoveInvItem)));
	Table.Add("Npc_RemoveInvItems", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_RemoveInvItems)));
	Table.Add("Npc_ClearInventory", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_ClearInventory)));
	Table.Add("Npc_GetEquippedMeleeWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetEquippedMeleeWeapon)));
	Table.Add("Npc_GetEquippedRangedWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetEquippedRangedWeapon)));
	Table.Add("Npc_GetEquippedArmor", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetEquippedArmor)));
	Table.Add("Npc_HasEquippedWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_HasEquippedWeapon)));
	Table.Add("Npc_HasEquippedMeleeWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_HasEquippedMeleeWeapon)));
	Table.Add("Npc_HasEquippedRangedWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_HasEquippedRangedWeapon)));
	Table.Add("Npc_HasEquippedArmor", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_HasEquippedArmor)));
	Table.Add("Npc_OwnedByNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_OwnedByNpc)));
	Table.Add("Npc_IsDetectedMobOwnedByNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsDetectedMobOwnedByNpc)));
	Table.Add("Npc_SetRefuseTalk", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetRefuseTalk)));
	Table.Add("Npc_RefuseTalk", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_RefuseTalk)));
	Table.Add("Npc_IsDead", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsDead)));
	Table.Add("Npc_KnowsInfo", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_KnowsInfo)));
	Table.Add("Npc_CheckInfo", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_CheckInfo)));
	Table.Add("Npc_IsPlayer", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsPlayer)));
	Table.Add("Npc_GetDistToNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetDistToNpc)));
	Table.Add("Npc_GetHeightToNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetHeightToNpc)));
	Table.Add("Npc_GetHeightToItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetHeightToItem)));
	Table.Add("Npc_GetDistToWP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetDistToWP)));
	Table.Add("Npc_SetTrueGuild", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetTrueGuild)));
	Table.Add("Npc_GetTrueGuild", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetTrueGuild)));
	Table.Add("Npc_SetAttitude", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetAttitude)));
	Table.Add("Npc_GetPermAttitude", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetPermAttitude)));
	Table.Add("Npc_SetTempAttitude", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetTempAttitude)));
	Table.Add("Npc_GetAttitude", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetAttitude)));
	Table.Add("Npc_GetGuildAttitude", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetGuildAttitude)));
	Table.Add("Npc_ClearAIQueue", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_ClearAIQueue)));
	Table.Add("Npc_IsInState", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsInState)));
	Table.Add("Npc_WasInState", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_WasInState)));
	Table.Add("Npc_ExchangeRoutine", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_ExchangeRoutine)));
	Table.Add("Npc_ChangeAttribute", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_ChangeAttribute)));
	Table.Add("Npc_GetLastHitSpellID", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetLastHitSpellID)));
	Table.Add("Npc_GetLastHitSpellCat", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetLastHitSpellCat)));
	Table.Add("Npc_SetActiveSpellInfo", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetActiveSpellInfo)));
	Table.Add("Npc_GetActiveSpellCat", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetActiveSpellCat)));
	Table.Add("Npc_PercEnable", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_PercEnable)));
	Table.Add("Npc_PercDisable", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_PercDisable)));
	Table.Add("Npc_SetPercTime", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SetPercTime)));
	Table.Add("Npc_SendPassivePerc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_SendPassivePerc)));
	Table.Add("Npc_PerceiveAll", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_PerceiveAll)));
	Table.Add("Npc_GetDetectedMob", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetDetectedMob)));
	Table.Add("Npc_CanSeeNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_CanSeeNpc)));
	Table.Add("Npc_CanSeeNpcFreeLOS", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_CanSeeNpcFreeLOS)));
	Table.Add("Npc_PlayAni", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_PlayAni)));
	Table.Add("Npc_GetPortalGuild", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_GetPortalGuild)));
	Table.Add("Npc_IsInPlayersRoom", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Npc_IsInPlayersRoom)));
	Table.Add("Log_CreateTopic", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Log_CreateTopic)));
	Table.Add("Log_SetTopicStatus", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Log_SetTopicStatus)));
	Table.Add("Log_AddEntry", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Log_AddEntry)));
	Table.Add("AI_Wait", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_Wait)));
	Table.Add("AI_PlayAni", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_PlayAni)));
	Table.Add("AI_StandUp", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_StandUp)));
	Table.Add("AI_StandUpQuick", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_StandUpQuick)));
	Table.Add("AI_LookAtNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_LookAtNpc)));
	Table.Add("AI_StopLookAt", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_StopLookAt)));
	Table.Add("AI_UseItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_UseItem)));
	Table.Add("AI_UseItemToState", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_UseItemToState)));
	Table.Add("AI_UseMob", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_UseMob)));
	Table.Add("AI_SetWalkmode", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_SetWalkmode)));
	Table.Add("AI_GotoWP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_GotoWP)));
	Table.Add("AI_GotoFP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_GotoFP)));
	Table.Add("AI_GotoNextFP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_GotoNextFP)));
	Table.Add("AI_GotoNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_GotoNpc)));
	Table.Add("AI_Teleport", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_Teleport)));
	Table.Add("AI_TurnToNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_TurnToNpc)));
	Table.Add("AI_AlignToWP", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_AlignToWP)));
	Table.Add("AI_Dodge", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_Dodge)));
	Table.Add("AI_PlayAniBS", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_PlayAniBS)));
	Table.Add("AI_RemoveWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_RemoveWeapon)));
	Table.Add("AI_DrawWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_DrawWeapon)));
	Table.Add("AI_ReadyMeleeWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_ReadyMeleeWeapon)));
	Table.Add("AI_ReadyRangedWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_ReadyRangedWeapon)));
	Table.Add("AI_Attack", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_Attack)));
	Table.Add("AI_FinishingMove", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_FinishingMove)));
	Table.Add("AI_EquipBestMeleeWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_EquipBestMeleeWeapon)));
	Table.Add("AI_EquipBestRangedWeapon", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_EquipBestRangedWeapon)));
	Table.Add("AI_EquipBestArmor", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_EquipBestArmor)));
	Table.Add("AI_UnequipWeapons", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_UnequipWeapons)));
	Table.Add("AI_UnequipArmor", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_UnequipArmor)));
	Table.Add("AI_EquipArmor", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_EquipArmor)));
	Table.Add("AI_Output", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_Output)));
	Table.Add("AI_OutputSVM", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_OutputSVM)));
	Table.Add("AI_OutputSVM_Overlay", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_OutputSVM_Overlay)));
	Table.Add("AI_StopProcessInfos", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_StopProcessInfos)));
	Table.Add("AI_StartState", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_StartState)));
	Table.Add("AI_StopAim", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_StopAim)));
	Table.Add("AI_SetNpcsToState", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_SetNpcsToState)));
	Table.Add("AI_ContinueRoutine", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_ContinueRoutine)));
	Table.Add("AI_ReadySpell", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, AI_ReadySpell)));
	Table.Add("CreateInvItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, CreateInvItem)));
	Table.Add("CreateInvItems", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, CreateInvItems)));
	Table.Add("EquipItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, EquipItem)));
	Table.Add("Mob_HasItems", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mob_HasItems)));
	Table.Add("Wld_GetDay", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_GetDay)));
	Table.Add("Wld_IsTime", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_IsTime)));
	Table.Add("Wld_InsertNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_InsertNpc)));
	Table.Add("Wld_IsMobAvailable", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_IsMobAvailable)));
	Table.Add("Wld_GetMobState", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_GetMobState)));
	Table.Add("Wld_IsFPAvailable", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_IsFPAvailable)));
	Table.Add("Wld_IsNextFPAvailable", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_IsNextFPAvailable)));
	Table.Add("Wld_InsertItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_InsertItem)));
	Table.Add("Wld_SetGuildAttitude", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_SetGuildAttitude)));
	Table.Add("Wld_GetGuildAttitude", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_GetGuildAttitude)));
	Table.Add("Wld_DetectNpc", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_DetectNpc)));
	Table.Add("Wld_DetectNpcEx", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_DetectNpcEx)));
	Table.Add("Wld_DetectItem", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_DetectItem)));
	Table.Add("Wld_AssignRoomToGuild", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_AssignRoomToGuild)));
	Table.Add("Wld_GetPlayerPortalGuild", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_GetPlayerPortalGuild)));
	Table.Add("Wld_GetFormerPlayerPortalGuild", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_GetFormerPlayerPortalGuild)));
	Table.Add("Wld_SpawnNpcRange", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Wld_SpawnNpcRange)));
	Table.Add("Mdl_ApplyOverlayMds", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mdl_ApplyOverlayMds)));
	Table.Add("Mdl_RemoveOverlayMDS", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mdl_RemoveOverlayMDS)));
	Table.Add("Mdl_ApplyOverlayMDSTimed", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mdl_ApplyOverlayMDSTimed)));
	Table.Add("Mdl_SetModelScale", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mdl_SetModelScale)));
	Table.Add("Mdl_SetModelFatness", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mdl_SetModelFatness)));
	Table.Add("Doc_Create", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_Create)));
	Table.Add("Doc_CreateMap", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_CreateMap)));
	Table.Add("Doc_SetPages", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_SetPages)));
	Table.Add("Doc_SetPage", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_SetPage)));
	Table.Add("Doc_SetMargins", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_SetMargins)));
	Table.Add("Doc_SetFont", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_SetFont)));
	Table.Add("Doc_SetLevel", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_SetLevel)));
	Table.Add("Doc_SetLevelCoords", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_SetLevelCoords)));
	Table.Add("Doc_PrintLine", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_PrintLine)));
	Table.Add("Doc_PrintLines", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_PrintLines)));
	Table.Add("Doc_Show", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Doc_Show)));
	Table.Add("TA_Min", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, TA_Min)));
	Table.Add("ExitGame", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, ExitGame)));
	Table.Add("ExitSession", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, ExitSession)));
	Table.Add("PlayVideo", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, PlayVideo)));
	Table.Add("PlayVideoEx", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, PlayVideoEx)));
	Table.Add("IntroduceChapter", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, IntroduceChapter)));
	Table.Add("Snd_Play", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Snd_Play)));
	Table.Add("Snd_Play3D", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Snd_Play3D)));
	Table.Add("Snd_GetDistToSource", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Snd_GetDistToSource)));
	Table.Add("Game_InitGerman", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Game_InitGerman)));
	Table.Add("Game_InitEnglish", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Game_InitEnglish)));
	Table.Add("PrintDebugInst", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, PrintDebugInst)));
	Table.Add("PrintDebugInstCh", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, PrintDebugInstCh)));
	Table.Add("PrintDebugCh", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, PrintDebugCh)));

	// Custom???
	Table.Add("PrintDebug", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, PrintDebug)));
	Table.Add("Mdl_SetVisual", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mdl_SetVisual)));
	Table.Add("Mdl_SetVisualBody", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, Mdl_SetVisualBody)));

	//Table.Add("create_dial_widget", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, CreateDialWidget)));

	//Table.Add("get_property", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, UnrealGetProperty)));
	//Table.Add("set_property", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UGHLuaState, UnrealSetProperty)));

	static ConstructorHelpers::FClassFinder<ACharacter> NPC_Blueprint(TEXT("/Game/Game/NPC"));
	NpcClass = NPC_Blueprint.Class;

	static ConstructorHelpers::FClassFinder<AActor> Entity_Blueprint(TEXT("/Game/Game/Entity"));
	EntityClass = Entity_Blueprint.Class;
	
	// auto Script = NewObject<UGHScript>();
	// Script->Initialization(this);
}

void UGHLuaState::LuaStateInit()
{
	UserDataMetaTable = CreateLuaTable();
	UserDataMetaTable.SetField("__index", UGHLuaState::MetaMethodIndex_C);
	UserDataMetaTable.SetField("__newindex", UGHLuaState::MetaMethodNewIndex_C);
	UserDataMetaTable.SetField("__eq", UGHLuaState::MetaMethodEq_C);
	UserDataMetaTable.SetField("__tostring", UGHLuaState::MetaMethodToString_C);
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::LuaStateInit");

	//auto Script = NewObject<UGHScript>();
	//Script->Initialization(this);
}

// Object deep copy
FLuaValue UGHLuaState::CreateLuaObject(FLuaValue Instance)
{
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	
	const auto LuaValueFunction = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, StaticClass(), "create_lua_object");
	if (LuaValueFunction.Type != ELuaValueType::Function)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueFunction.Type != ELuaValueType::Function"));
		return FLuaValue();
	}
	auto LuaValueNewInstance = ULuaBlueprintFunctionLibrary::LuaGlobalCallValue(World,
		StaticClass(), LuaValueFunction, {Instance});
	if (LuaValueNewInstance.Type == ELuaValueType::Nil)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueNewInstance.Type == ELuaValueType::Nil"));
		return FLuaValue();
	}

	return LuaValueNewInstance;
}

TArray<FLuaValue> UGHLuaState::MetaMethodIndex(TArray<FLuaValue> LuaArgs)
{
	TArray<FLuaValue> ReturnValues;

	UObject* Object = LuaArgs[0].Object;
	FString Key = LuaArgs[1].ToString();

	// skip nullptr and classes
	if (!Object || Object->IsA<UClass>())
	{
		return ReturnValues;
	}

	ELuaReflectionType ReflectionType = ELuaReflectionType::Unknown;
	ULuaBlueprintFunctionLibrary::GetLuaReflectionType(Object, Key, ReflectionType);

	if (ReflectionType == ELuaReflectionType::Property)
	{
		ReturnValues.Add(GetLuaValueFromProperty(Object, Key));
	}
	else if (ReflectionType == ELuaReflectionType::Function)
	{
		ReturnValues.Add(FLuaValue::FunctionOfObject(Object, FName(Key)));
	}
	else if (ReflectionType == ELuaReflectionType::Unknown)
	{
		UStruct* Class = Cast<UStruct>(Object);
		if (!Class)
			Class = Object->GetClass();
		
		FProperty* Property_Table = Class->FindPropertyByName(FLuaValue("_table").ToName());
		if (!Property_Table)
			return ReturnValues;

		if (Class->GetName() == NpcClass->GetName())
		{
			AGHNPC* Npc = Cast<AGHNPC>(Object);
			if (Npc->_table.Type != ELuaValueType::Table)
				return ReturnValues;
			
			ReturnValues.Add(Npc->_table.GetField(Key));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Unknown Class");
		}
	}

	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "MetaMethodIndex, Key " + Key);

	return ReturnValues;
}

TArray<FLuaValue> UGHLuaState::MetaMethodNewIndex(TArray<FLuaValue> LuaArgs)
{
	TArray<FLuaValue> ReturnValues;

	UObject* Object = LuaArgs[0].Object;
	FString Key = LuaArgs[1].ToString();
	FLuaValue Value = LuaArgs[2];

	// skip nullptr and classes
	if (!Object || Object->IsA<UClass>())
	{
		return ReturnValues;
	}

	ELuaReflectionType ReflectionType = ELuaReflectionType::Unknown;
	ULuaBlueprintFunctionLibrary::GetLuaReflectionType(Object, Key, ReflectionType);

	if (ReflectionType == ELuaReflectionType::Property)
	{
		SetPropertyFromLuaValue(Object, Key, Value);
	}
	else if (ReflectionType == ELuaReflectionType::Unknown)
	{
		UStruct* Class = Cast<UStruct>(Object);
		if (!Class)
			Class = Object->GetClass();
		
		FProperty* Property_Table = Class->FindPropertyByName(FLuaValue("_table").ToName());
		if (!Property_Table)
			return ReturnValues;

		if (Class->GetName() == NpcClass->GetName())
		{
			AGHNPC* Npc = Cast<AGHNPC>(Object);
			if (Npc->_table.Type != ELuaValueType::Table)
				return ReturnValues;
			
			Npc->_table.SetField(Key, Value);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Unknown Class");
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "MetaMethodNewIndex, Key " + Key);

	return ReturnValues;
}

TArray<FLuaValue> UGHLuaState::MetaMethodEq(TArray<FLuaValue> LuaArgs)
{
	TArray<FLuaValue> ReturnValues;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "MetaMethodEq");

	UObject* Object = LuaArgs[0].Object;
	UObject* OtherObject = LuaArgs[1].Object;

	if (!Object || !OtherObject)
	{
		ReturnValues.Add(FLuaValue(false));
	}
	else
	{
		ReturnValues.Add(FLuaValue(Object == OtherObject));
	}

	return ReturnValues;
}

TArray<FLuaValue> UGHLuaState::MetaMethodToString(TArray<FLuaValue> LuaArgs)
{
	TArray<FLuaValue> ReturnValues;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "MetaMethodToString");

	UObject* Object = LuaArgs[0].Object;

	// skip nullptr and classes
	if (!Object || Object->IsA<UClass>())
	{
		return ReturnValues;
	}

	ReturnValues.Add(Object->GetFullName());

	return ReturnValues;
}

//
// Script Functions
//

// Returns a newly constructed string object with its value being the concatenation of the characters in lhs followed by those of rhs.
// string ConcatStrings(string lhs, string rhs)
FLuaValue UGHLuaState::ConcatStrings(FLuaValue Lhs, FLuaValue Rhs)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::ConcatStrings");

	auto LeftString = Lhs.ToString();
	auto RightString = Rhs.ToString();

	return FLuaValue(LeftString + RightString);
}

// A string object containing the representation of 'x' as a sequence of characters.
// string IntToString(int x)
FLuaValue UGHLuaState::IntToString(FLuaValue X)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::IntToString");

	if (!(X.Type == ELuaValueType::Integer || X.Type == ELuaValueType::Number))
		return FLuaValue();

	auto XInt = X.ToInteger();
	
	return FLuaValue(XInt);
}

// A string object containing the representation of 'x' as a sequence of characters.
// int FloatToString(float x)
FLuaValue UGHLuaState::FloatToString(FLuaValue X)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::FloatToString");

	if (!(X.Type == ELuaValueType::Integer || X.Type == ELuaValueType::Number))
		return FLuaValue();
	
	return FLuaValue(X.ToInteger());
}

// Return random value in range [0..max-1]. Retunrns 0, if max=0
// int Hlp_Random(int max)
FLuaValue UGHLuaState::Hlp_Random(FLuaValue Max)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::Hlp_Random");

	if (!(Max.Type == ELuaValueType::Integer || Max.Type == ELuaValueType::Number))
		return FLuaValue();
	
	int32 MaxInt = Max.ToInteger() - 1;
	if (MaxInt < 0)
		MaxInt = 0;
	auto RandomInt = FMath::RandRange(0, MaxInt);

	return FLuaValue(RandomInt);
}

// Retunrn True, if s1 is equal to s2; False otherwise
// int Hlp_StrCmp(string s1, string s2)
FLuaValue UGHLuaState::Hlp_StrCmp(FLuaValue S1, FLuaValue S2)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::Hlp_StrCmp");

	return FLuaValue(S1.ToString() == S2.ToString());
}

// Returns True, if 'self' is valid reference to C_Npc; False otherwise
// int Hlp_IsValidNpc(C_Npc self)
FLuaValue UGHLuaState::Hlp_IsValidNpc(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::Hlp_IsValidNpc");

	if (LuaValueNpc.Type != ELuaValueType::UObject)
		return FLuaValue(false);

	auto ChekedNpc = Cast<AGHNPC>(LuaValueNpc.Object);
	if (!ChekedNpc)
		return FLuaValue(false);
	
	auto NpcTable = ChekedNpc->_table;
	if (NpcTable.Type != ELuaValueType::Table)
		return FLuaValue(false);

	auto TypeField = NpcTable.GetField("__type");
	if (TypeField.Type != ELuaValueType::String)
		return FLuaValue(false);

	if (TypeField.ToString() != "C_Npc")
		return FLuaValue(false);

	auto InstanceField = NpcTable.GetField("__instance");
	if (InstanceField.Type != ELuaValueType::String)
		return FLuaValue(false);
	
	return FLuaValue(true);
}

// Returns True, if 'item' is valid reference to C_Item; False otherwise
// int Hlp_IsValidItem(C_Item item)
FLuaValue UGHLuaState::Hlp_IsValidItem(FLuaValue Item)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::Hlp_IsValidItem");

	if (Item.Type != ELuaValueType::UObject)
		return FLuaValue(false);

	auto ChekedItem = Cast<AGHEntity>(Item.Object);
	if (!ChekedItem)
		return FLuaValue(false);
	
	auto NpcTable = ChekedItem->_table;
	if (NpcTable.Type != ELuaValueType::Table)
		return FLuaValue(false);

	auto TypeField = NpcTable.GetField("__type");
	if (TypeField.Type != ELuaValueType::String)
		return FLuaValue(false);

	if (TypeField.ToString() != "C_Item")
		return FLuaValue(false);

	auto InstanceField = NpcTable.GetField("__instance");
	if (InstanceField.Type != ELuaValueType::String)
		return FLuaValue(false);
	
	return FLuaValue(true);
}

// Returns True, if 'item' is valid reference to C_Item and belongs to instanceName 'class'; False otherwise
// Example usage "if( Hlp_IsItem(item,ItMuArrow)==TRUE ) { ... }"
// int Hlp_IsItem(C_Item item, int instanceName)
FLuaValue UGHLuaState::Hlp_IsItem(FLuaValue Item, FLuaValue ItemInstance)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::Hlp_IsItem");

	if (Item.Type != ELuaValueType::UObject)
		return FLuaValue(false);

	auto ChekedItem = Cast<AGHEntity>(Item.Object);
	if (!ChekedItem)
		return FLuaValue(false);
	
	auto ItemTable = ChekedItem->_table;
	if (ItemTable.Type != ELuaValueType::Table)
		return FLuaValue(false);

	auto TypeField = ItemTable.GetField("__type");
	if (TypeField.Type != ELuaValueType::String)
		return FLuaValue(false);

	if (TypeField.ToString() != "C_Item")
		return FLuaValue(false);

	auto InstanceField = ItemTable.GetField("__instance");
	if (InstanceField.Type != ELuaValueType::String)
		return FLuaValue(false);

	// ---

	if (ItemInstance.Type != ELuaValueType::Table)
		return FLuaValue(false);
	
	FLuaValue InstanceTypeField = ItemInstance.GetField("__type");
	if (InstanceTypeField.Type != ELuaValueType::String)
		return FLuaValue(false);

	if (InstanceTypeField.ToString() != "C_Npc")
		return FLuaValue(false);

	FLuaValue InstanceInstanceField = ItemInstance.GetField("__instance");
	if (InstanceInstanceField.Type != ELuaValueType::String)
		return FLuaValue(false);

	if (InstanceField.ToString() == InstanceInstanceField.ToString())
		return FLuaValue(true);

	return FLuaValue(false);
}

// Return any C_Npc instance of class instanceName. Returns invalid reference, if no instances found.
// C_Npc Hlp_GetNpc(int instanceName)
FLuaValue UGHLuaState::Hlp_GetNpc(FLuaValue LuaValueNpcInstance)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Hlp_GetNpc: " + LuaValueNpcInstance.ToString());

	if (LuaValueNpcInstance.Type != ELuaValueType::Table)
		return FLuaValue();
	
	FLuaValue InstanceTypeField = LuaValueNpcInstance.GetField("__type");
	if (InstanceTypeField.Type != ELuaValueType::String)
		return FLuaValue();

	if (InstanceTypeField.ToString() != "C_Npc")
		return FLuaValue();

	FLuaValue InstanceInstanceField = LuaValueNpcInstance.GetField("__instance");
	if (InstanceInstanceField.Type != ELuaValueType::String)
		return FLuaValue();
	
	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	const auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode)
		return FLuaValue();

	for (auto Npc : GameMode->NpcArray)
	{
		if (!Npc)
			continue;

		FLuaValue NpcTable = Npc->_table;
		if (NpcTable.Type != ELuaValueType::Table)
			continue;

		FLuaValue TypeField = NpcTable.GetField("__type");
		if (TypeField.Type != ELuaValueType::String)
			continue;

		if (TypeField.ToString() != "C_Npc")
			continue;

		FLuaValue InstanceField = NpcTable.GetField("__instance");
		if (InstanceField.Type != ELuaValueType::String)
			continue;

		if (InstanceField.ToString() != InstanceInstanceField.ToString())
			continue;

		return FLuaValue(Npc);
	}
	
	// TArray<AActor*> FoundActors;
	// UGameplayStatics::GetAllActorsOfClass(GWorld, NpcClass, FoundActors);
	// for (AActor* Actor : FoundActors)
	// {
	// 	AGHNPC* Npc = Cast<AGHNPC>(Actor);
	// 	if (Npc)
	// 	{
	// 		FLuaValue LuaValueNpcTable = Npc->_table;
	// 		if (NpcTable.Type != ELuaValueType::Table)
	// 			continue;
	//
	// 		FLuaValue TypeField = NpcTable.GetField("__type");
	// 		if (TypeField.Type != ELuaValueType::String)
	// 			continue;
	//
	// 		if (TypeField.ToString() != "C_Npc")
	// 			continue;
	//
	// 		FLuaValue InstanceField = NpcTable.GetField("__instance");
	// 		if (InstanceField.Type != ELuaValueType::String)
	// 			continue;
	//
	// 		if (InstanceField.ToString() != InstanceInstanceField.ToString())
	// 			continue;
	//
	// 		return FLuaValue(Npc);
	// 	}
	// }
	
	return FLuaValue();
}

// Return instance-id of npc
// int Hlp_GetInstanceID(C_Npc npc)
FLuaValue UGHLuaState::Hlp_GetInstanceID(FLuaValue Object)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::Hlp_GetInstanceID");

	if (Object.Type != ELuaValueType::UObject)
		return FLuaValue();
	
	auto ChekedNpc = Cast<AGHNPC>(Object.Object);
	if (ChekedNpc)
	{
		auto NpcTable = ChekedNpc->_table;
		if (NpcTable.Type != ELuaValueType::Table)
			return FLuaValue();

		auto TypeField = NpcTable.GetField("__type");
		if (TypeField.Type != ELuaValueType::String)
			return FLuaValue();

		if (TypeField.ToString() != "C_Npc")
			return FLuaValue();

		auto InstanceField = NpcTable.GetField("__instance");
		if (InstanceField.Type != ELuaValueType::String)
			return FLuaValue();

		auto IdField = NpcTable.GetField("id");
		if (IdField.Type == ELuaValueType::Integer || IdField.Type == ELuaValueType::Number)
			return IdField;
	}
	
	auto ChekedEntity = Cast<AGHEntity>(Object.Object);
	if (ChekedEntity)
	{
		auto EntityTable = ChekedEntity->_table;
		if (EntityTable.Type != ELuaValueType::Table)
			return FLuaValue();

		auto TypeField = EntityTable.GetField("__type");
		if (TypeField.Type != ELuaValueType::String)
			return FLuaValue();

		if (TypeField.ToString() != "C_Item")
			return FLuaValue();

		auto InstanceField = EntityTable.GetField("__instance");
		if (InstanceField.Type != ELuaValueType::String)
			return FLuaValue();

		auto IdField = EntityTable.GetField("id");
		if (IdField.Type == ELuaValueType::Integer || IdField.Type == ELuaValueType::Number)
			return IdField;
	}

	return FLuaValue();
}

// // Return instance-id of item
// // int Hlp_GetInstanceID(C_Item item)
// FLuaValue UGHLuaState::Hlp_GetInstanceID(FLuaValue Item)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::Hlp_GetInstanceID");
//
// 	return FLuaValue();
// }

// Return how much time 'npc' spent in current state
// int Npc_GetStateTime(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetStateTime(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("Npc_GetStateTime: %s"), *LuaValueNpc.ToString());

	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue(0);
	}

	return FLuaValue(Npc->CurrentStateTime);
}

// Overrides state-time for 'npc'
// void Npc_SetStateTime(C_Npc npc, int seconds)
void UGHLuaState::Npc_SetStateTime(FLuaValue LuaValueNpc, FLuaValue Seconds)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SetStateTime");

	// AI
}

// Return name of nearest to 'npc' waypoint
// string Npc_GetNearestWP(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetNearestWP(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("Npc_GetNearestWP: %s"), *LuaValueNpc.ToString());
	
	auto CheckedNpc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!CheckedNpc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}
	
	auto Location = CheckedNpc->GetActorLocation();
	auto Waypoint = Helpers::GetNearestWaypoint(Location);
	
	return FLuaValue(Waypoint.Name);
}

// Return name of second nearest to 'npc' waypoint
// string Npc_GetNextWP(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetNextWP(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetNextWP");

	return FLuaValue();
}

// Return current bodystate of 'npc', as bitset.
// Not user-friendly, use helper function C_BodyStateContains(self,bodystate)
// int Npc_GetBodyState(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetBodyState(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetBodyState");

	// AI
	return FLuaValue();
}

// Checks npc state for specified flag. Most used argument for bodyFlag is BS_FLAG_INTERRUPTABLE.
// int Npc_HasBodyFlag(C_Npc npc, int bodyFlag)
FLuaValue UGHLuaState::Npc_HasBodyFlag(FLuaValue LuaValueNpc, FLuaValue Bodyflag)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_HasBodyFlag");

	// AI
	return FLuaValue();
}

// Set npc to melee fight-mode, with no weapon in hands
// void Npc_SetToFistMode(C_Npc npc)
void UGHLuaState::Npc_SetToFistMode(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SetToFistMode");

	// AI
}

// Set npc to melee fight-mode, with weapon, specified by 'weaponId', in hands.
// Weapon will be created automatically, if npc doesn't have it in inventory
// void Npc_SetToFightMode(C_Npc npc, int weaponId)
void UGHLuaState::Npc_SetToFightMode(FLuaValue LuaValueNpc, FLuaValue Weaponid)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SetToFightMode");

	// AI
}

// Return TRUE, if npc have appropriate weapon in hands.
// Note: fmode is not a bitset!
// int Npc_IsInFightMode(C_Npc self, int fmode)
FLuaValue UGHLuaState::Npc_IsInFightMode(FLuaValue LuaValueNpc, FLuaValue LuaValueFightMode)
{
	UE_LOG(Lua, Log, TEXT("Npc_IsInFightMode: %s, %s"), *LuaValueNpc.ToString(), *LuaValueFightMode.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue(false); // ??
	}
	
	if (!(LuaValueFightMode.Type == ELuaValueType::Integer || LuaValueFightMode.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueFightMode.Type != ELuaValueType::Integer||Number"));
		return FLuaValue(false); // ??
	}
	const auto FightMode = LuaValueFightMode.ToInteger();
	
	return FLuaValue(FightMode == static_cast<int32>(Npc->CurrentFightMode));
}

// Returns current active weapon(sword for FMODE_MELEE, bow for FMODE_FAR, etc)
// C_Item Npc_GetReadiedWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetReadiedWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetReadiedWeapon");

	// AI
	return FLuaValue();
}

// Note: partial implementation
// Returns currently drawing weapon(sword for FMODE_MELEE, bow for FMODE_FAR, etc)
// bool Npc_IsDrawingWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_IsDrawingWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_IsDrawingWeapon");

	// NOT USE
	return FLuaValue();
}

// Returns TRUE, if npc has melee weapon in hands.
// int Npc_HasReadiedMeleeWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_HasReadiedMeleeWeapon(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("Npc_HasReadiedMeleeWeapon: %s"), *LuaValueNpc.ToString());

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue(false);
	}

	auto LuaValueWeaponInHands = Npc->ItemInHands;
	if (LuaValueWeaponInHands.Type != ELuaValueType::Table)
	{
		// No log, its ok
		return FLuaValue(false);
	}

	auto LuaValueItemCategory = LuaValueWeaponInHands.GetField("mainflag");
	if (!(LuaValueItemCategory.Type == ELuaValueType::Integer || LuaValueItemCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueItemCategory.Type != ELuaValueType::Integer||Number"));
		return FLuaValue(false);
	}

	auto ItemCategory = LuaValueItemCategory.ToInteger();
	if (ItemCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		return FLuaValue(true);
	}
	
	return FLuaValue(false);
}

// If, npc had a internal target to attack, then function returns TRUE, and assign global variable 'other' to npc.target.
// Returns FALSE, if npc.target is empty.
// int Npc_GetTarget(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetTarget(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetTarget");

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
		return FLuaValue(false);
	
	auto LuaValueTarget = Npc->_table.GetField("target");
	if (LuaValueNpc.Type != ELuaValueType::UObject)
		return FLuaValue(false);

	auto Target = Helpers::GetNpcFromLuaValue(LuaValueTarget);
	if (!Target)
		return FLuaValue(false);

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	
	ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, UGHLuaState::StaticClass(), "other", FLuaValue(Target));
	
	return FLuaValue(true);
}

// Perform search for a closest enemy(see ATTITUDE api). The enemy must not be dead, or be in unconscious state.
// Returns TRUE and save result to 'other' and npc.target, if target is found.
// FALSE, if target not found.
// Note: in original Gothic you have to call Npc_PerceiveAll, before process any search; OpenGothic doesn't care.
// int Npc_GetNextTarget(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetNextTarget(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetNextTarget");

	// AI
	return FLuaValue();
}

// Assign internal reference npc.target to 'other'.
// void Npc_SetTarget(C_Npc npc, C_Npc other)
void UGHLuaState::Npc_SetTarget(FLuaValue LuaValueNpc, FLuaValue LuaValueOther)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SetTarget");
	
	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
		return;

	auto Other = Helpers::GetNpcFromLuaValue(LuaValueOther);
	if (!Other)
		return;
	
	Npc->_table.SetField("target", FLuaValue(Other));
}

// Returns TRUE, if 'npc' is standing of fp with name 'fpname'.
// int Npc_IsOnFP(C_Npc npc, string fpname)
FLuaValue UGHLuaState::Npc_IsOnFP(FLuaValue LuaValueNpc, FLuaValue Fpname)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_IsOnFP");

	return FLuaValue();
}

// if item with id "itemInstance" exists in inventory of "npc" then:
// returns the item and assign global variable "item" to it.
// Otherwise - "item" set to invalid reference.
// C_Item Npc_GetInvItem(C_Npc npc, int itemInstance)
FLuaValue UGHLuaState::Npc_GetInvItem(FLuaValue LuaValueNpc, FLuaValue Iteminstance)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetInvItem");

	return FLuaValue();
}

// Return count of items with class 'itemInstance' in 'npc' inventory.
// int Npc_HasItems(C_Npc npc, int itemInstance)
FLuaValue UGHLuaState::Npc_HasItems(FLuaValue LuaValueNpc, FLuaValue LuaValueItemInstance)
{
	UE_LOG(Lua, Log, TEXT("Npc_HasItems: %s"), *LuaValueNpc.ToString());

	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue(0);
	}

	const auto bIsInstance = Helpers::CheckInstance(LuaValueItemInstance);
	if (!bIsInstance)
	{
		UE_LOG(Lua, Warning, TEXT("!bIsInstance"));
		return FLuaValue(0);
	}

	auto LuaValueItems = Npc->_table.GetField("items");
	if (LuaValueItems.Type != ELuaValueType::Table)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueItems.Type != ELuaValueType::Table"));
		return FLuaValue(0);
	}

	auto LuaValueItemInstanceName = LuaValueItemInstance.GetField("__instance").ToString();
	int32 LuaValueWeaponCount = 0;
	auto LuaValueItemArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(LuaValueItems);
	for (auto LuaValueItem : LuaValueItemArray)
	{
		if (LuaValueItem.Type != ELuaValueType::Table)
			continue;

		auto InstanceFieldName = LuaValueItem.GetField("__instance").ToString();
		if (LuaValueItemInstanceName == InstanceFieldName)
		{
			LuaValueWeaponCount++;
		}
	}

	// TODO add multi
	
	return FLuaValue(LuaValueWeaponCount);
}

// Shortcut for Npc_RemoveInvItems(npc,itemInstance,1)
// void Npc_RemoveInvItem(C_Npc npc, int itemInstance)
void UGHLuaState::Npc_RemoveInvItem(FLuaValue LuaValueNpc, FLuaValue Iteminstance)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_RemoveInvItem");

}

// Removes 'amount' of items with class 'itemInstance' from 'npc' inventory.
// this function takes no effect, if amount&lt;1.
// void Npc_RemoveInvItems(C_Npc npc, int itemInstance, int amount)
void UGHLuaState::Npc_RemoveInvItems(FLuaValue LuaValueNpc, FLuaValue Iteminstance, FLuaValue Amount)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_RemoveInvItems");

}

// Remove all items from inventory of 'npc', except mission items and equipped items.
// int Npc_ClearInventory(C_Npc npc)
FLuaValue UGHLuaState::Npc_ClearInventory(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_ClearInventory");

	return FLuaValue();
}

// Returns current melee weapon of 'npc'.
// C_Item Npc_GetEquippedMeleeWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetEquippedMeleeWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetEquippedMeleeWeapon");

	return FLuaValue();
}

// Returns current ranged weapon of 'npc'.
// C_Item Npc_GetEquippedRangedWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetEquippedRangedWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetEquippedRangedWeapon");

	return FLuaValue();
}

// Returns current armour of 'npc'.
// C_Item Npc_GetEquippedArmor(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetEquippedArmor(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetEquippedArmor");

	return FLuaValue();
}

// Returns TRUE, if 'npc' has any equipped weapon.
// Same as: "Npc_HasEquippedMeleeWeapon(npc) || Npc_HasEquippedRangedWeapon(npc)"
// int Npc_HasEquippedWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_HasEquippedWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_HasEquippedWeapon");

	return FLuaValue();
}

// Returns TRUE, if 'npc' has equipped melee weapon.
// int Npc_HasEquippedMeleeWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_HasEquippedMeleeWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_HasEquippedMeleeWeapon");

	return FLuaValue();
}

// Returns TRUE, if 'npc' has equipped ranged weapon.
// int Npc_HasEquippedRangedWeapon(C_Npc npc)
FLuaValue UGHLuaState::Npc_HasEquippedRangedWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_HasEquippedRangedWeapon");

	return FLuaValue();
}

// Returns TRUE, if 'npc' has equipped armor.
// int Npc_HasEquippedArmor(C_Npc npc)
FLuaValue UGHLuaState::Npc_HasEquippedArmor(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_HasEquippedArmor");

	return FLuaValue();
}

// Returns TRUE, if 'npc' is owner of 'item'.
// int Npc_OwnedByNpc(C_Item item, C_Npc npc)
FLuaValue UGHLuaState::Npc_OwnedByNpc(FLuaValue Item, FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_OwnedByNpc");

	return FLuaValue();
}

// Returns TRUE, if 'user' is currently use an mob owned by 'npc'.
// int Npc_IsDetectedMobOwnedByNpc(C_Npc npc, C_Npc user)
FLuaValue UGHLuaState::Npc_IsDetectedMobOwnedByNpc(FLuaValue LuaValueNpc, FLuaValue User)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_IsDetectedMobOwnedByNpc");

	return FLuaValue();
}

// 'npc' is awailable for talk for next 'timeSec' seconds
// take no effect, if timeSec&lt;1
// void Npc_SetRefuseTalk(C_Npc npc, int timeSec)
void UGHLuaState::Npc_SetRefuseTalk(FLuaValue LuaValueNpc, FLuaValue Timesec)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SetRefuseTalk");

}

// Returns TRUE, if npc still refuses to talk
// int Npc_RefuseTalk(C_Npc npc)
FLuaValue UGHLuaState::Npc_RefuseTalk(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_RefuseTalk");

	return FLuaValue();
}

// Returns TRUE, if 'npc' state is ZS_DEAD
// int Npc_IsDead(C_Npc npc)
FLuaValue UGHLuaState::Npc_IsDead(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("Npc_IsDead: %s"), *LuaValueNpc.ToString());

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
		return FLuaValue(true); // ???
	
	return FLuaValue(Npc->CurrentStateName.Contains("ZS_Dead"));
}

// Returns TRUE, if 'npc' knowns information with id = 'infoId'
// int Npc_KnowsInfo(C_Npc npc, int infoId)
FLuaValue UGHLuaState::Npc_KnowsInfo(FLuaValue LuaValueNpc, FLuaValue Infoid)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_KnowsInfo");

	return FLuaValue();
}

// Returns TRUE, if 'npc' has any info to tell, with important level equals 'important'
// int Npc_CheckInfo(C_Npc npc, int important)
FLuaValue UGHLuaState::Npc_CheckInfo(FLuaValue LuaValueNpc, FLuaValue Important)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_CheckInfo");

	return FLuaValue();
}

// Return TRUE, if 'npc' is the player
// int Npc_IsPlayer(C_Npc npc)
FLuaValue UGHLuaState::Npc_IsPlayer(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_IsPlayer");

	return FLuaValue();
}

// Return distance from 'npc1' to 'npc2' in santimeters.
// int Npc_GetDistToNpc(C_Npc npc1, C_Npc npc2)
FLuaValue UGHLuaState::Npc_GetDistToNpc(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2)
{
	UE_LOG(Lua, Log, TEXT("Npc_GetDistToNpc: %s, %s"), *LuaValueNpc1.ToString(), *LuaValueNpc2.ToString());
	
	const auto Npc1 = Helpers::GetNpcFromLuaValue(LuaValueNpc1);
	if (!Npc1)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc1"));
		return FLuaValue(0);
	}

	const auto Npc2 = Helpers::GetNpcFromLuaValue(LuaValueNpc2);
	if (!Npc2)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc2"));
		return FLuaValue(0);
	}

	const auto Npc1Location = Npc1->GetActorLocation();
	const auto Npc2Location = Npc2->GetActorLocation();
	const int32 Dist = FMath::FloorToInt(FVector::Dist(Npc1Location, Npc2Location));
	
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Npc_GetDistToNpc: " + FString::FromInt(Dist));
	
	return FLuaValue(Dist);
}

// Return distance by Y axis from 'npc1' to 'npc2' in santimeters.
// int Npc_GetHeightToNpc(C_Npc npc1, C_Npc npc2)
FLuaValue UGHLuaState::Npc_GetHeightToNpc(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetHeightToNpc");

	return FLuaValue();
}

// Return distance by Y axis from 'npc1' to 'itm' in santimeters.
// int Npc_GetHeightToItem(C_Npc npc1, C_Item itm)
FLuaValue UGHLuaState::Npc_GetHeightToItem(FLuaValue LuaValueNpc1, FLuaValue Itm)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetHeightToItem");

	return FLuaValue();
}

// Return distance from 'npc' to waypoint with name 'wpName'.
// int Npc_GetDistToWP(C_Npc npc, string wpName)
FLuaValue UGHLuaState::Npc_GetDistToWP(FLuaValue LuaValueNpc, FLuaValue LuaValueWaypointName)
{
	UE_LOG(Lua, Log, TEXT("Npc_GetDistToWP: %s, %s"), *LuaValueNpc.ToString(), *LuaValueWaypointName.ToString());
	
	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}

	if (LuaValueWaypointName.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueWaypointName.Type != ELuaValueType::String"));
		return FLuaValue(0);
	}
	
	auto NpcLocation = Npc->GetActorLocation();

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	auto WaypointName = LuaValueWaypointName.ToString();
	auto Waypoint = GameMode->WaypointMap.Find(WaypointName);
	if (!Waypoint)
	{
		UE_LOG(Lua, Warning, TEXT("!Waypoint"));
		return FLuaValue(0);
	}
	
	float Dist = FVector::Dist(NpcLocation, Waypoint->Position);
	
	return FLuaValue(Dist);
}

// Set 'true' guildId for 'npc'
// int Npc_SetTrueGuild(C_Npc npc, int guildId)
FLuaValue UGHLuaState::Npc_SetTrueGuild(FLuaValue LuaValueNpc, FLuaValue LuaValueGuild)
{
	UE_LOG(Lua, Log, TEXT("Npc_SetTrueGuild: %s, %s"), *LuaValueNpc.ToString(), *LuaValueGuild.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}
	
	if (!(LuaValueGuild.Type == ELuaValueType::Integer || LuaValueGuild.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueGuild.Type != LuaValueGuild::Integer||Number"));
		return FLuaValue();
	}

	Npc->TrueGuild = LuaValueGuild;
	
	return LuaValueGuild;
}

// Return 'true' guildId for 'npc'
// int Npc_GetTrueGuild(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetTrueGuild(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("Npc_SetAttitude: %s"), *LuaValueNpc.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}

	if (Npc->TrueGuild.Type == ELuaValueType::Integer || Npc->TrueGuild.Type == ELuaValueType::Number)
	{
		return Npc->TrueGuild;
	}

	const auto LuaValueGuild = Npc->_table.GetField("guild");
	if (!(LuaValueGuild.Type == ELuaValueType::Integer || LuaValueGuild.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueGuild.Type != LuaValueGuild::Integer||Number"));
		return FLuaValue();
	}
	
	return LuaValueGuild;
}

// https://worldofplayers.ru/threads/13039/page-129#post-809656
// Set attitude for npc to player (Perm???)
// void Npc_SetAttitude(C_Npc npc, int att)
void UGHLuaState::Npc_SetAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueAttitude)
{
	UE_LOG(Lua, Log, TEXT("Npc_SetAttitude: %s, %s"), *LuaValueNpc.ToString(), *LuaValueAttitude.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	if (!(LuaValueAttitude.Type == ELuaValueType::Integer || LuaValueAttitude.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueAttitude.Type != LuaValueAttitude::Integer||Number"));
		return;
	}
	
	const auto PlayerNpc = Helpers::GetPlayerNpc();
	if (!PlayerNpc)
	{
		UE_LOG(Lua, Warning, TEXT("!PlayerNpc"));
		return;
	}

	const auto InstanceName = PlayerNpc->_table.GetField("__instance").ToString();
	const auto Attitude = static_cast<EAttitude>(LuaValueAttitude.ToInteger());

	// Npc->AttitudePerm[InstanceName] = Attitude;
	Npc->AttitudePerm.Remove(InstanceName);
	Npc->AttitudePerm.Add(InstanceName, Attitude);
}

// https://worldofplayers.ru/threads/13039/page-129#post-809656
// Return permanent attitude for npc to other
// int Npc_GetPermAttitude(C_Npc npc, C_Npc other)
FLuaValue UGHLuaState::Npc_GetPermAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueOther)
{
	UE_LOG(Lua, Log, TEXT("Npc_GetPermAttitude: %s, %s"), *LuaValueNpc.ToString(), *LuaValueOther.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}

	const auto Other = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Other)
	{
		UE_LOG(Lua, Warning, TEXT("!Other"));
		return FLuaValue();
	}

	const auto InstanceName = Other->_table.GetField("__instance").ToString();
	auto Attitude = EAttitude::Hostile; // for easy b_u_g detect

	if (Npc->AttitudePerm.Contains(InstanceName))
	{
		Attitude = Npc->AttitudePerm.FindRef(InstanceName);
		return FLuaValue(static_cast<int32>(Attitude));
	}
	else if (Npc->AttitudeGuild.Contains(InstanceName))
	{
		Attitude = Npc->AttitudeGuild.FindRef(InstanceName);
		return FLuaValue(static_cast<int32>(Attitude));
	}

	return FLuaValue(static_cast<int32>(Attitude));
}

// Set temporary attitude for npc to player,
// void Npc_SetTempAttitude(C_Npc npc, int att)
void UGHLuaState::Npc_SetTempAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueAttitude)
{
	UE_LOG(Lua, Log, TEXT("Npc_SetTempAttitude: %s, %s"), *LuaValueNpc.ToString(), *LuaValueAttitude.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	if (!(LuaValueAttitude.Type == ELuaValueType::Integer || LuaValueAttitude.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueAttitude.Type != LuaValueAttitude::Integer||Number"));
		return;
	}
	
	const auto PlayerNpc = Helpers::GetPlayerNpc();
	if (!PlayerNpc)
	{
		UE_LOG(Lua, Warning, TEXT("!PlayerNpc"));
		return;
	}

	const auto InstanceName = PlayerNpc->_table.GetField("__instance").ToString();
	const auto Attitude = static_cast<EAttitude>(LuaValueAttitude.ToInteger());

	// Npc->AttitudeTemp[InstanceName] = Attitude;
	Npc->AttitudeTemp.Remove(InstanceName);
	Npc->AttitudeTemp.Add(InstanceName, Attitude);

	const auto World = GEngine->GetCurrentPlayWorld(nullptr);
	
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([Npc, InstanceName]
	{
		Npc->AttitudeTemp.Remove(InstanceName);
	}), 120.0f, false); // TODO change time to gametime
}

// Return attitude betwen 'npc' and 'other'
// Gibt die Attitüde von self zu other zurück (temp. / perm. / Gilden )
// int Npc_GetAttitude(C_Npc npc, C_Npc other)
FLuaValue UGHLuaState::Npc_GetAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueOther)
{
	UE_LOG(Lua, Log, TEXT("Npc_GetAttitude: %s, %s"), *LuaValueNpc.ToString(), *LuaValueOther.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}

	const auto Other = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Other)
	{
		UE_LOG(Lua, Warning, TEXT("!Other"));
		return FLuaValue();
	}

	const auto InstanceName = Other->_table.GetField("__instance").ToString();
	auto Attitude = EAttitude::Hostile; // for easy b_u_g detect

	if (Npc->AttitudeTemp.Contains(InstanceName))
	{
		Attitude = Npc->AttitudeTemp.FindRef(InstanceName);
		return FLuaValue(static_cast<int32>(Attitude));
	}
	else if (Npc->AttitudePerm.Contains(InstanceName))
	{
		Attitude = Npc->AttitudePerm.FindRef(InstanceName);
		return FLuaValue(static_cast<int32>(Attitude));
	}
	else if (Npc->AttitudeGuild.Contains(InstanceName))
	{
		Attitude = Npc->AttitudeGuild.FindRef(InstanceName);
		return FLuaValue(static_cast<int32>(Attitude));
	}

	return FLuaValue(static_cast<int32>(Attitude));
}

// Return attitude betwen guilds of 'npc' and 'other'
// int Npc_GetGuildAttitude(C_Npc npc, C_Npc other)
FLuaValue UGHLuaState::Npc_GetGuildAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueOther)
{
	UE_LOG(Lua, Log, TEXT("Npc_GetGuildAttitude: %s, %s"), *LuaValueNpc.ToString(), *LuaValueOther.ToString());
	
	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}

	const auto Other = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Other)
	{
		UE_LOG(Lua, Warning, TEXT("!Other"));
		return FLuaValue();
	}

	const auto InstanceName = Other->_table.GetField("__instance").ToString();
	auto Attitude = EAttitude::Hostile; // for easy b_u_g detect

	if (Npc->AttitudeGuild.Contains(InstanceName))
	{
		Attitude = Npc->AttitudeGuild.FindRef(InstanceName);
		return FLuaValue(static_cast<int32>(Attitude));
	}

	return FLuaValue(static_cast<int32>(Attitude));
}

// Removes every queued action from npc ai-queue
// void Npc_ClearAIQueue(C_Npc npc)
void UGHLuaState::Npc_ClearAIQueue(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("Npc_ClearAIQueue: %s"), *LuaValueNpc.ToString());

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	Npc->State = EAIState::Stop;
}

// Return TRUE, if current state of 'npc' is equal to 'state_fn'
// int Npc_IsInState(C_Npc npc, func state_fn)
FLuaValue UGHLuaState::Npc_IsInState(FLuaValue LuaValueNpc, FLuaValue StateName)
{
	UE_LOG(Lua, Log, TEXT("Npc_IsInState: %s, %s"), *LuaValueNpc.ToString(), *StateName.ToString());

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue(false);
	}
	
	return FLuaValue(Npc->CurrentStateName == StateName.ToString());
}

// Return TRUE, if previous state of 'npc' is equal to 'state_fn'
// int Npc_WasInState(C_Npc npc, func state_fn)
FLuaValue UGHLuaState::Npc_WasInState(FLuaValue LuaValueNpc, FLuaValue StateName)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_WasInState");

	return FLuaValue();
}

// Clear routines list of npc and create a new by calling function Rtn_{routineName}_{npc.id}
// void Npc_ExchangeRoutine(C_Npc npc, string routineName)
void UGHLuaState::Npc_ExchangeRoutine(FLuaValue LuaValueNpc, FLuaValue Routinename)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_ExchangeRoutine");

}

// Sets a new 'value' to attribute 'att' if 'npc'. if value of 'att' is not in range [0..7], this function takes no effect.
// void Npc_ChangeAttribute(C_Npc npc, int atr, int value)
void UGHLuaState::Npc_ChangeAttribute(FLuaValue LuaValueNpc, FLuaValue Atr, FLuaValue Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_ChangeAttribute");

}

// Return Id of spell witch hit 'npc' last
// int Npc_GetLastHitSpellID(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetLastHitSpellID(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetLastHitSpellID");

	return FLuaValue();
}

// Return category of spell witch hit 'npc' last
// int Npc_GetLastHitSpellCat(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetLastHitSpellCat(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetLastHitSpellCat");

	return FLuaValue();
}

// Set extra information related to spell. Used only for transform spells: "Npc_SetActiveSpellInfo(self, DragonSnapper)"
// int Npc_SetActiveSpellInfo(C_Npc npc, int i1)
FLuaValue UGHLuaState::Npc_SetActiveSpellInfo(FLuaValue LuaValueNpc, FLuaValue I1)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SetActiveSpellInfo");

	return FLuaValue();
}

// Return category of active spell, for caster 'npc'
// int Npc_GetActiveSpellCat(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetActiveSpellCat(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetActiveSpellCat");

	return FLuaValue();
}

// assign a new perception-procedure to 'npc'. 'percId' has to be one of enum values, described bellow:
// void Npc_PercEnable(C_Npc npc, int percId, func callback)
void UGHLuaState::Npc_PercEnable(FLuaValue LuaValueNpc, FLuaValue Percid, FLuaValue Callback)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_PercEnable");

}

// clear perception callback with 'percId' index for 'npc'.
// void Npc_PercDisable(C_Npc npc, int percId)
void UGHLuaState::Npc_PercDisable(FLuaValue LuaValueNpc, FLuaValue Percid)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_PercDisable");

}

// Set timer interval for perceptions of 'npc'
// void Npc_SetPercTime(C_Npc npc, float seconds)
void UGHLuaState::Npc_SetPercTime(FLuaValue LuaValueNpc, FLuaValue Seconds)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SetPercTime");

}

// make an invokation of "npc.perc[percId]", with preinitialized global variables 'victum' and 'other'.
// The invocation is asynchronous.
// Example:
// void Npc_SendPassivePerc(C_Npc npc, int percId, C_Npc victum, C_Npc other)
void UGHLuaState::Npc_SendPassivePerc(FLuaValue LuaValueNpc, FLuaValue Percid, FLuaValue Victum, FLuaValue Other)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_SendPassivePerc");

}

// Implemented as nop.
// void Npc_PerceiveAll(C_Npc npc)
void UGHLuaState::Npc_PerceiveAll(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_PerceiveAll");

	// really dont need it?
}

// Returns scheme name of MOB, whitch is currently in use by 'npc'. If 'npc' is not using any of MOB returns empty string.
// string Npc_GetDetectedMob(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetDetectedMob(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetDetectedMob");

	return FLuaValue();
}

// Returns TRUE, if npc2 is visible from 'npc1' point of view.
// This fuction also check 'npc1' view direction, with +- 100 degress margin.
// int Npc_CanSeeNpc(C_Npc npc1, C_Npc npc2)
FLuaValue UGHLuaState::Npc_CanSeeNpc(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_CanSeeNpc");

	return FLuaValue();
}

// Returns TRUE, if 'npc2' is visible from 'npc1' point of view, without checking view direction.
// int Npc_CanSeeNpcFreeLOS(C_Npc npc1, C_Npc npc2)
FLuaValue UGHLuaState::Npc_CanSeeNpcFreeLOS(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_CanSeeNpcFreeLOS");

	return FLuaValue();
}

// Play animation 'anim', for 'npc'.
// void Npc_PlayAni(C_Npc npc, string anim)
void UGHLuaState::Npc_PlayAni(FLuaValue LuaValueNpc, FLuaValue Anim)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_PlayAni");

}

// if 'npc' located inside room, return guild assigned for this room.
// otherwise returns GIL_NONE
// int Npc_GetPortalGuild(C_Npc npc)
FLuaValue UGHLuaState::Npc_GetPortalGuild(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_GetPortalGuild");

	return FLuaValue();
}

// Returns TRUE, if 'npc' is in same room as player.
// int Npc_IsInPlayersRoom(C_Npc npc)
FLuaValue UGHLuaState::Npc_IsInPlayersRoom(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Npc_IsInPlayersRoom");

	return FLuaValue();
}

// Add a new entry in quest-log section described by section.
// Nop, if section argument is none of LOG_NOTE and LOG_MISSION
// void Log_CreateTopic(string name, int section)
void UGHLuaState::Log_CreateTopic(FLuaValue Name, FLuaValue Section)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Log_CreateTopic");

	if (Name.Type != ELuaValueType::String)
		return;

	if (!(Section.Type == ELuaValueType::Integer || Section.Type == ELuaValueType::Number))
		return;

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	auto TopicName = Name.ToString();
	if (GameMode->QuestMap.Contains(TopicName))
		return;

	auto Quest = FQuest();
	Quest.Name = TopicName;

	auto TopicTypeInt = Section.ToInteger();
	auto TopicType = static_cast<EQuestType>(TopicTypeInt);
	//if (TopicType.)
	Quest.Type = TopicType;

	GameMode->QuestMap.Add(TopicName, Quest);
}

// Set a new status for quest-log entry with name=="name"
// void Log_SetTopicStatus(string name, int status)
void UGHLuaState::Log_SetTopicStatus(FLuaValue Name, FLuaValue Status)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Log_SetTopicStatus");
	
	if (Name.Type != ELuaValueType::String)
		return;

	if (!(Status.Type == ELuaValueType::Integer || Status.Type == ELuaValueType::Number))
		return;

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	auto TopicName = Name.ToString();
	if (!GameMode->QuestMap.Contains(TopicName))
		return;

	auto Quest = GameMode->QuestMap.Find(TopicName);
	if (Quest)
	{
		auto TopicStatusInt = Status.ToInteger();
		auto TopicStatus = static_cast<EQuestStatus>(TopicStatusInt);
		Quest->Status = TopicStatus;
		//if (TopicStatus)
		//{
		//	Quest->Status = TopicStatus;
		//}
	}
}

// Add a new entry to quest-topic.
// void Log_AddEntry(string topic, string entry)
void UGHLuaState::Log_AddEntry(FLuaValue Name, FLuaValue Entry)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Log_AddEntry");
	
	if (Name.Type != ELuaValueType::String)
		return;

	if (Entry.Type != ELuaValueType::String)
		return;

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	auto TopicName = Name.ToString();
	if (!GameMode->QuestMap.Contains(TopicName))
		return;

	auto Quest = GameMode->QuestMap.Find(TopicName);
	if (Quest)
	{
		Quest->Records.Add(Entry.ToString());
	}
}

// Wait for 's' seconds
// void AI_Wait(C_Npc npc, float s)
void UGHLuaState::AI_Wait(FLuaValue LuaValueNpc, FLuaValue S)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_Wait");

}

// Play animation 'anim', for 'npc'.
// void AI_PlayAni(C_Npc npc, string anim)
void UGHLuaState::AI_PlayAni(FLuaValue LuaValueNpc, FLuaValue Anim)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_PlayAni");

}

// Dettach 'npc' from MOBSI object.
// Note: it's not clear how exactly this function should work.
// void AI_StandUp(C_Npc npc)
void UGHLuaState::AI_StandUp(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_StandUp");

}

// Same as AI_StandUp, but skips animation.
// void AI_StandUpQuick(C_Npc npc)
void UGHLuaState::AI_StandUpQuick(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("AI_StandUpQuick: %s"), *LuaValueNpc.ToString());

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	// TODO: some error here
	//Npc->AnimInstance->StopAllMontage(0.f);
}

// 'npc' starts look to 'other'.
// void AI_LookAtNpc(C_Npc npc, C_Npc other)
void UGHLuaState::AI_LookAtNpc(FLuaValue LuaValueNpc, FLuaValue Other)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_LookAtNpc");

}

// 'npc' stops looking to npc/point
// void AI_StopLookAt(C_Npc npc)
void UGHLuaState::AI_StopLookAt(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_StopLookAt");

}

// use item of class 'itemInstance'. Nop, if 'npc' doesn't have any of 'itemInstance' items.
// void AI_UseItem(C_Npc npc, int itemInstance)
void UGHLuaState::AI_UseItem(FLuaValue LuaValueNpc, FLuaValue Iteminstance)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_UseItem");

}

// Use item with extended animation chain. Example:
// "AI_UseItemToState(self,ItLsTorchFirespit,5)" produces sequence of animations:
// "T_FIRESPIT_STAND_2_S0 -&gt; T_FIRESPIT_S0_2_S1 -&gt; S_FIRESPIT_S1 -&gt; T_FIRESPIT_S1_2_S2 -&gt; S_FIRESPIT_S2 -&gt; T_FIRESPIT_S2_2_S3 -&gt; S_FIRESPIT_S3 -&gt; T_FIRESPIT_S3_2_S4 -&gt; T_FIRESPIT_S4_2_STAND"
// void AI_UseItemToState(C_Npc npc, int itemInstance, int state)
void UGHLuaState::AI_UseItemToState(FLuaValue LuaValueNpc, FLuaValue Iteminstance, FLuaValue State)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_UseItemToState");

}

// 'npc' stats using MOBSIS with scheme 'schemeName', if MOBSIS is not busy.
// int AI_UseMob(C_Npc npc, string schemeName, int targetState)
FLuaValue UGHLuaState::AI_UseMob(FLuaValue LuaValueNpc, FLuaValue Schemename, FLuaValue Targetstate)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_UseMob");

	return FLuaValue();
}

// Set 'npc' walkmode
// void AI_SetWalkmode(C_Npc npc, int mode)
void UGHLuaState::AI_SetWalkmode(FLuaValue LuaValueNpc, FLuaValue LuaValueWalkMode)
{
	UE_LOG(Lua, Log, TEXT("AI_SetWalkmode: %s, %s"), *LuaValueNpc.ToString(), *LuaValueWalkMode.ToString());

	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}
	
	if (!(LuaValueWalkMode.Type == ELuaValueType::Integer || LuaValueWalkMode.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueWalkMode.Type != LuaValueWalkMode::Integer||Number"));
		return;
	}

	Npc->SetWalkMode(static_cast<EWalkMode>(LuaValueWalkMode.ToInteger()));
}

// 'npc' stats walk to WayPoint with name 'wpname'. The path is going to be calculated, by using waynet.
// void AI_GotoWP(C_Npc npc, string wpname)
void UGHLuaState::AI_GotoWP(FLuaValue LuaValueNpc, FLuaValue LuaValueWaypointName)
{
	UE_LOG(Lua, Log, TEXT("AI_GotoWP: LuaValueNpc: %s, LuaValueWaypointName: %s"),
		*LuaValueNpc.ToString(), *LuaValueWaypointName.ToString());
	
	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	if (LuaValueWaypointName.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueWaypointName.Type != ELuaValueType::String"));
		return;
	}

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	auto WaypointName = LuaValueWaypointName.ToString();
	auto Waypoint = GameMode->WaypointMap.Find(WaypointName);
	if (!Waypoint)
	{
		UE_LOG(Lua, Warning, TEXT("!Waypoint"));
		return;
	}

	// Npc->MoveToWaypoint(*Waypoint);
	
	auto NpcLocation = Npc->GetActorLocation();
	
	auto WaypointNameArray = Helpers::CalculatePathToWaypoint(NpcLocation, *Waypoint);
	for (auto WaypointNextName : WaypointNameArray)
	{
		auto WaypointNext = GameMode->WaypointMap.Find(WaypointNextName);
		if (WaypointNext)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, WaypointNextName);
			Npc->MoveWaypointArray.Add(*WaypointNext);
		}
	}
}

// 'npc' stats walk to FreePoint with name 'fpname'. Point must be available.
// The point must be less then 20 meters away, from npc.
// Once 'npc' start mode toward FP, point going to became 'locked'.
// void AI_GotoFP(C_Npc npc, string fpname)
void UGHLuaState::AI_GotoFP(FLuaValue LuaValueNpc, FLuaValue Fpname)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_GotoFP");

	// zCVobSpot
}

// 'npc' stats walk to FreePoint with name 'fpname'.
// Similar to AI_GotoFP, but 'current' point of 'npc' is going to be excluded from search.
// void AI_GotoNextFP(C_Npc npc, string fpname)
void UGHLuaState::AI_GotoNextFP(FLuaValue LuaValueNpc, FLuaValue Fpname)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_GotoNextFP");

}

// 'npc' stats walk to 'other'.
// void AI_GotoNpc(C_Npc npc, C_Npc other)
void UGHLuaState::AI_GotoNpc(FLuaValue LuaValueNpc, FLuaValue LuaValueOther)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_GotoNpc");

}

// 'npc' teleports to 'waypoint'.
// void AI_Teleport(C_NPC npc, string waypoint)
void UGHLuaState::AI_Teleport(FLuaValue LuaValueNpc, FLuaValue Waypoint)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_Teleport");

}

// 'npc' turns to 'other'.
// void AI_TurnToNpc(C_Npc npc, C_Npc other)
void UGHLuaState::AI_TurnToNpc(FLuaValue LuaValueNpc, FLuaValue Other)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_TurnToNpc");

}

// 'npc' turns to direction specified by current npc-waypoint.
// richtet den Nsc am Waypoint aus
// void AI_AlignToWP(C_Npc npc)
void UGHLuaState::AI_AlignToWP(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_AlignToWP");

}

// 'npc' make one step backward.
// void AI_Dodge(C_Npc npc)
void UGHLuaState::AI_Dodge(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_Dodge");

}

// 'npc' start a new animation sequence with name='aniname'.
// "bodystate" is going to be preserved across whole animation chain; Use "Npc_GetBodyState" current "npc" body-state.
// void AI_PlayAniBS(C_Npc npc, string aniname, int bodystate)
void UGHLuaState::AI_PlayAniBS(FLuaValue LuaValueNpc, FLuaValue Aniname, FLuaValue Bodystate)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_PlayAniBS");

}

// 'npc' removes any active weapon.
// void AI_RemoveWeapon(C_Npc npc)
void UGHLuaState::AI_RemoveWeapon(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("AI_RemoveWeapon: %s"), *LuaValueNpc.ToString());

	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	auto LuaValueWeaponInHands = Npc->ItemInHands;
	if (LuaValueWeaponInHands.Type != ELuaValueType::Table)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueWeaponInHands.Type != ELuaValueType::Table"));
		return;
	}

	const auto LuaValueWeaponCategory = LuaValueWeaponInHands.GetField("mainflag");
	if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer || LuaValueWeaponCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer||Number"));
		return;
	}
	
	const auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
	if (WeaponCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		Npc->RemoveWeaponLeftBelt();
	}
	// TODO: else if range weapon
}

// 'npc' draws any weapon
// void AI_DrawWeapon(C_Npc npc)
void UGHLuaState::AI_DrawWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_DrawWeapon");

}

// 'npc' draws melee weapon.
// void AI_ReadyMeleeWeapon(C_Npc npc)
void UGHLuaState::AI_ReadyMeleeWeapon(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("AI_ReadyMeleeWeapon: %s"), *LuaValueNpc.ToString());

	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	auto LuaValueEquipedMeleeWeapon = Npc->EquippedMeleeWeapon;
	if (LuaValueEquipedMeleeWeapon.Type != ELuaValueType::Table)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueEquipedMeleeWeapon.Type != ELuaValueType::Table"));
		return;

		// TODO:ready fist, by default  Npc->EquipedMeleeWeapon == fist...
	}

	auto LuaValueWeaponCategory = LuaValueEquipedMeleeWeapon.GetField("mainflag");
	if (!(LuaValueWeaponCategory.Type == ELuaValueType::Integer || LuaValueWeaponCategory.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueWeaponCategory.Type != ELuaValueType::Integer||Number"));
		return;
	}
	
	auto WeaponCategory = LuaValueWeaponCategory.ToInteger();
	if (WeaponCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
	{
		Npc->ReadyMeleeWeapon();
	}
}

// 'npc' draws bow or crossbow.
// void AI_ReadyRangedWeapon(C_Npc npc)
void UGHLuaState::AI_ReadyRangedWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_ReadyRangedWeapon");

}

// Fetch next instruction from C_FightAI, corresponding to npc.fight_tactic
// void AI_Attack(C_Npc npc)
void UGHLuaState::AI_Attack(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_Attack");

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
		return;

	Npc->State = EAIState::Attack;
}

// npc performs killing move on 'other'.
// void AI_FinishingMove(C_Npc npc, C_Npc other)
void UGHLuaState::AI_FinishingMove(FLuaValue LuaValueNpc, FLuaValue Other)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_FinishingMove");

}

// Equips 'npc' with best melee weapon from his inventory.
// void AI_EquipBestMeleeWeapon(C_Npc npc)
void UGHLuaState::AI_EquipBestMeleeWeapon(FLuaValue LuaValueNpc)
{
	UE_LOG(Lua, Log, TEXT("AI_EquipBestMeleeWeapon: %s"), *LuaValueNpc.ToString());

	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	auto LuaValueItems = Npc->_table.GetField("items");
	if (LuaValueItems.Type != ELuaValueType::Table)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueItems.Type != ELuaValueType::Table"));
		return;
	}

	int32 MaximumDamage = 0;
	int32 Index = 0;
	int32 LuaValueBestMeleeWeaponIndex = 0;
	auto LuaValueBestMeleeWeapon = FLuaValue();
	auto LuaValueItemArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(LuaValueItems);
	for (auto LuaValueItem : LuaValueItemArray)
	{
		if (LuaValueItem.Type != ELuaValueType::Table)
			continue;
		
		auto LuaValueItemCategory = LuaValueItem.GetField("mainflag");
		if (!(LuaValueItemCategory.Type == ELuaValueType::Integer || LuaValueItemCategory.Type == ELuaValueType::Number))
			continue;

		auto LuaValueItemDamage = LuaValueItem.GetField("damageTotal");
		if (!(LuaValueItemCategory.Type == ELuaValueType::Integer || LuaValueItemCategory.Type == ELuaValueType::Number))
			continue;
		
		const auto ItemCategory = LuaValueItemCategory.ToInteger();
		if (ItemCategory & static_cast<int32>(EItemCategory::MeleeWeapon))
		{
			if (LuaValueItemDamage.ToInteger() > MaximumDamage)
			{
				MaximumDamage = LuaValueItemDamage.ToInteger();
				LuaValueBestMeleeWeaponIndex = Index;
				LuaValueBestMeleeWeapon = LuaValueItem;
			}
		}

		Index++;
	}

	if (MaximumDamage > 0)
	{
		// Npc->UseItemFromIndex(LuaValueBestMeleeWeaponIndex);
		Npc->EquipMesh(LuaValueBestMeleeWeapon);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "weapon max damage: " + FString::FromInt(MaximumDamage));
	}
	else
	{
		// TODO: set fist
	}
}

// Equips 'npc' with best range weapon from his inventory.
// void AI_EquipBestRangedWeapon(C_Npc npc)
void UGHLuaState::AI_EquipBestRangedWeapon(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_EquipBestRangedWeapon");
}

// Equips 'npc' with best armor from his inventory.
// void AI_EquipBestArmor(C_Npc npc)
void UGHLuaState::AI_EquipBestArmor(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_EquipBestArmor");

}

// Unequips all current weapons of 'npc'.
// void AI_UnequipWeapons(C_Npc npc)
void UGHLuaState::AI_UnequipWeapons(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_UnequipWeapons");

}

// Unequips current armor of 'npc'.
// void AI_UnequipArmor(C_Npc npc)
void UGHLuaState::AI_UnequipArmor(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_UnequipArmor");

}

// Equips 'npc' with armor specified by 'itemId'. Armor with class 'itemId' must be in 'npc' inventory.
// void AI_EquipArmor(C_Npc owner, int itemId)
void UGHLuaState::AI_EquipArmor(FLuaValue Owner, FLuaValue Itemid)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_EquipArmor");

}

// 'npc' tells to 'target' a dialog line determinated by 'outputName'.
// 'npc' also starts a random dialog animation.
// void AI_Output(C_Npc npc, C_Npc target, string outputName)
void UGHLuaState::AI_Output(FLuaValue LuaValueNpc, FLuaValue Target, FLuaValue OutputName)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_Output");

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	GameMode->Dialog->CreateWidgetDialogText(LuaValueNpc, Target, OutputName);
}

// Similar to AI_Output, but sound and text is determinated by SVM subsystem
// Similar to AI_OutputSVM, but works wih no animation.
// void AI_OutputSVM(C_Npc npc, C_Npc target, string svmname)
void UGHLuaState::AI_OutputSVM(FLuaValue LuaValueNpc, FLuaValue Target, FLuaValue Svmname)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_OutputSVM");

}

// Similar to AI_OutputSVM, but works wih no animation.
// void AI_OutputSVM_Overlay(C_Npc npc, C_Npc target, string svmname)
void UGHLuaState::AI_OutputSVM_Overlay(FLuaValue LuaValueNpc, FLuaValue Target, FLuaValue Svmname)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_OutputSVM_Overlay");

}

// Stop dialog mode for player and 'npc'.
// Takes no effect, if player doesn't speek with 'npc'.
// void AI_StopProcessInfos(C_Npc npc)
void UGHLuaState::AI_StopProcessInfos(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_StopProcessInfos");

}

// starts a new state for npc, described by 'state' function.
// if 'doFinalize' is non zero, current 'npc' state is going to be finalized, by 'STATE_End' function.
// In Gothic state is triple of functions: init, loop, end.
// void AI_StartState(C_Npc npc, func state, int doFinalize, string wpName)
void UGHLuaState::AI_StartState(FLuaValue LuaValueNpc, FLuaValue StateFunctionName, FLuaValue IsFinalize, FLuaValue Waypoint)
{
	UE_LOG(Lua, Log, TEXT("AI_StartState: %s, %s"), *LuaValueNpc.ToString(), *StateFunctionName.ToString());

	const auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}
	
	if (StateFunctionName.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("StateFunctionName.Type != ELuaValueType::String"));
		return;
	}

	const auto World = GEngine->GetCurrentPlayWorld(nullptr);

	// Can start state without start
	auto LuaValueStateFunction = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World,
		StaticClass(), StateFunctionName.ToString());

	// Can start state without loop
	auto LuaValueStateFunctionLoop = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World,
		StaticClass(), StateFunctionName.ToString() + "_Loop");

	auto bIsFinalize = IsFinalize.Type == ELuaValueType::Bool && IsFinalize.ToBool();
	auto LuaValueStateFunctionEnd = FLuaValue();
	if (bIsFinalize)
	{
		// Can start state without end
		LuaValueStateFunctionEnd = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World,
			StaticClass(), StateFunctionName.ToString() + "_End");
	}

	Npc->StartStateLoop(StateFunctionName.ToString(), LuaValueStateFunction,
		LuaValueStateFunctionLoop, LuaValueStateFunctionEnd, Waypoint);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_StartState all ok");
}

// НПС attacker прекращает целиться из оружия дальнего радиуса поражения.
// void AI_StopAim (c_npc attacker);
void UGHLuaState::AI_StopAim(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_StopAim");
}

// Set every C_Npc around 'npc' to new state described by 'aiStateFunc'.
// Not recomended to use in original Gothic.
// void AI_SetNpcsToState(C_Npc npc, fnuc aiStateFunc, int radius)
void UGHLuaState::AI_SetNpcsToState(FLuaValue LuaValueNpc, FLuaValue Aistatefunc, FLuaValue Radius)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_SetNpcsToState");

}

// 'npc' returns back to his dayly routine.
// void AI_ContinueRoutine(C_Npc npc)
void UGHLuaState::AI_ContinueRoutine(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_ContinueRoutine");
}

// 'npc' draw a spell with specified 'spellId'.
// void AI_ReadySpell(C_Npc npc, int spellID, int investMana)
void UGHLuaState::AI_ReadySpell(FLuaValue LuaValueNpc, FLuaValue Spellid, FLuaValue Investmana)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::AI_ReadySpell");

}

// Shortcut for CreateInvItems(npc,itemId,1).
// void CreateInvItem(C_Npc npc, int itemId)
FLuaValue UGHLuaState::CreateInvItem(FLuaValue& LuaValueNpc, FLuaValue& LuaValueItemInstance)
{
	UE_LOG(Lua, Log, TEXT("CreateInvItem: %s, %s"), *LuaValueNpc.ToString(), *LuaValueItemInstance.ToString());

	auto LuaValueNumber = FLuaValue(1);
	return CreateInvItems(LuaValueNpc, LuaValueItemInstance, LuaValueNumber);
}

// Add 'number' of items with class described by 'itemId' to 'npc' inventory.
// Affect result of hlp_getinstanceid(itemId).
// This function akes no effect, if 'number' = 1 : fixed
// void CreateInvItems(C_Npc npc, int itemId, int number)
FLuaValue UGHLuaState::CreateInvItems(FLuaValue& LuaValueNpc, FLuaValue& LuaValueItemInstance, FLuaValue& LuaValueNumber)
{
	UE_LOG(Lua, Log, TEXT("CreateInvItems: %s, %s, %s"), *LuaValueNpc.ToString(),
		*LuaValueItemInstance.ToString(), *LuaValueNumber.ToString());

	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return FLuaValue();
	}

	auto bIsInstance = Helpers::CheckInstance(LuaValueItemInstance);
	if (!bIsInstance)
	{
		UE_LOG(Lua, Warning, TEXT("!bIsInstance"));
		return FLuaValue();
	}
	
	if (!(LuaValueNumber.Type == ELuaValueType::Integer || LuaValueNumber.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueNumber.Type != Integer||Number"));
		return FLuaValue();
	}

	return Npc->InventoryCreateItems(LuaValueItemInstance, LuaValueNumber.ToInteger());
}

// Equip 'npc' with, an item of 'itemId' class. Can be used for armor/weapons/runes/etc
// if Npc_HasItems(npc,itemId)==0, new item will be created  inside of 'npc' inventory.
// Calls C_Item.on_equip callback, of specified item.
// Function do nothing, if this item is already equipped.
// void EquipItem(C_Npc npc, int itemId)
void UGHLuaState::EquipItem(FLuaValue LuaValueNpc, FLuaValue LuaValueItemInstance)
{
	UE_LOG(Lua, Log, TEXT("EquipItem: %s, %s"), *LuaValueNpc.ToString(), *LuaValueItemInstance.ToString());

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	if (!World)
	{
		UE_LOG(Lua, Warning, TEXT("!World"));
		return;
	}
	
	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	FString ItemInstanceName;
	if (LuaValueItemInstance.Type == ELuaValueType::String)
	{
		ItemInstanceName = LuaValueItemInstance.ToString();
		
		LuaValueItemInstance = Helpers::GetGlobalLuaValue(ItemInstanceName);
		if (!Helpers::CheckInstance(LuaValueItemInstance, C_ITEM))
		{
			UE_LOG(Lua, Warning, TEXT("%s it's not item instance"), *ItemInstanceName);
			return;
		}
	}
	else if (LuaValueItemInstance.Type == ELuaValueType::Table)
	{
		if (!Helpers::CheckInstance(LuaValueItemInstance, C_ITEM))
		{
			UE_LOG(Lua, Warning, TEXT("%s it's not item instance"), *ItemInstanceName);
			return;
		}

		ItemInstanceName = LuaValueItemInstance.GetField("__instance").ToString();
		
		if (!Helpers::RecheckGlobalInstance(ItemInstanceName))
		{
			UE_LOG(Lua, Warning, TEXT("%s wrong instance"), *ItemInstanceName);
			return;
		}
	}

	if (ItemInstanceName.IsEmpty())
	{
		UE_LOG(Lua, Warning, TEXT("ItemInstanceName.IsEmpty()"));
	}
	else
	{
		UE_LOG(Lua, Warning, TEXT("total item instance: %s"), *ItemInstanceName);
		
		UE_LOG(Lua, Warning, TEXT("----------------------"));
		auto LuaValueKeyArray = ULuaBlueprintFunctionLibrary::LuaTableGetKeys(Npc->_table);
		auto LuaValueValueArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Npc->_table);
		if (LuaValueKeyArray.Num() == LuaValueValueArray.Num())
		{
			for (int32 i = 0; i < LuaValueKeyArray.Num(); i++)
			{
				UE_LOG(Lua, Warning, TEXT("NPC, key: %s, value: %s"),
					*LuaValueKeyArray[i].ToString(), *LuaValueValueArray[i].ToString());
			}
		}
		
		Npc->ActionQueue->EquipItemActionAdd(ItemInstanceName);
	}

	// add co queue

	

	// auto LuaValueOldSelf = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, StaticClass(), "self");

	// need ckeck global...
	
	//
	// auto bIsInstance = Helpers::CheckInstance(LuaValueItemInstance);
	// if (!bIsInstance)
	// {
	// 	UE_LOG(Lua, Warning, TEXT("!bIsInstance"));
	// 	return;
	// }
	//
	// UE_LOG(Lua, Log, TEXT("EquipItem1: %s, %s"), *LuaValueNpc.ToString(), *LuaValueItemInstance.ToString());
	//
	// auto LuaValueItems = Npc->_table.GetField("items");
	// if (LuaValueItems.Type != ELuaValueType::Table)
	// {
	// 	UE_LOG(Lua, Warning, TEXT("LuaValueItems.Type != ELuaValueType::Table"));
	// 	return;
	// }
	//
	// auto LuaValueItemInstanceName = LuaValueItemInstance.GetField("__instance").ToString();
	// auto LuaValueItemArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(LuaValueItems);
	// for (auto LuaValueItem : LuaValueItemArray)
	// {
	// 	if (LuaValueItem.Type != ELuaValueType::Table)
	// 		continue;
	//
	// 	auto InstanceFieldName = LuaValueItem.GetField("__instance").ToString();
	// 	if (LuaValueItemInstanceName == InstanceFieldName)
	// 	{
	// 		auto LuaValueEquipped = LuaValueItem.GetField("equipped");
	// 		if (LuaValueEquipped.Type == ELuaValueType::Bool && LuaValueEquipped.ToBool())
	// 		{
	// 			// already have and equipped
	// 			return;
	// 		}
	// 		else
	// 		{
	// 			// TODO: equip ITEM!!!
	// 			// equipped only
	// 			Npc->EquipWeapon(LuaValueItem);
	// 		
	// 			return;
	// 		}
	// 	}
	// }
	// UE_LOG(Lua, Log, TEXT("EquipItem2: %s, %s"), *LuaValueNpc.ToString(), *LuaValueItemInstance.ToString());
	//
	// auto LuaValueCreatedItem = CreateInvItem(LuaValueNpc, LuaValueItemInstance);
	// if (LuaValueCreatedItem.Type != ELuaValueType::Table)
	// {
	// 	UE_LOG(Lua, Warning, TEXT("LuaValueCreatedItem.Type != ELuaValueType::Table"));
	// 	return;
	// }
	//
	// UE_LOG(Lua, Log, TEXT("EquipItem3: %s, %s"), *LuaValueNpc.ToString(), *LuaValueItemInstance.ToString());
	//
	// auto LuaValueKeyArray = ULuaBlueprintFunctionLibrary::LuaTableGetKeys(LuaValueCreatedItem);
	// auto LuaValueValueArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(LuaValueCreatedItem);
	// if (LuaValueKeyArray.Num() == LuaValueValueArray.Num())
	// {
	// 	for (int32 i = 0; i < LuaValueKeyArray.Num(); i++)
	// 	{
	// 		UE_LOG(Lua, Warning, TEXT("item, key: %s,     value: %s"),
	// 			*LuaValueKeyArray[i].ToString(), *LuaValueValueArray[i].ToString());
	// 	}
	// }
	//
	// Npc->EquipWeapon(LuaValueCreatedItem);
}

// Return count of items with class 'itemId', inside of the chest with name 'mobName'
// int Mob_HasItems(string mobName, int itemId)
FLuaValue UGHLuaState::Mob_HasItems(FLuaValue Mobname, FLuaValue Itemid)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Mob_HasItems");

	return FLuaValue();
}

// Return days from start of adventure, starting from zero.
// int Wld_GetDay()
FLuaValue UGHLuaState::Wld_GetDay()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_GetDay");

	return FLuaValue();
}

// Return TRUE, if current game time is in range [S .. E) or [E .. S).
// int Wld_IsTime(int hourS, int minS, int hourE, int minE)
FLuaValue UGHLuaState::Wld_IsTime(FLuaValue Hours, FLuaValue Mins, FLuaValue Houre, FLuaValue Mine)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_IsTime");

	return FLuaValue();
}

// Inserts an C_Npc object in world, at 'spawnPoint' location.
// void Wld_InsertNpc(int npcInstance, string spawnPoint)
FLuaValue UGHLuaState::Wld_InsertNpc(FLuaValue LuaValueNpcInstance, FLuaValue LuaValueSpawnPoint)
{
	UE_LOG(Lua, Log, TEXT("Wld_InsertNpc: LuaValueNpcInstance: %s, LuaValueSpawnPoint: %s"),
		*LuaValueNpcInstance.ToString(), *LuaValueSpawnPoint.ToString());

	if (LuaValueNpcInstance.Type != ELuaValueType::Table)
		return FLuaValue();
	
	FLuaValue InstanceTypeField = LuaValueNpcInstance.GetField("__type");
	if (InstanceTypeField.Type != ELuaValueType::String)
		return FLuaValue();

	if (InstanceTypeField.ToString() != "C_Npc")
		return FLuaValue();

	FLuaValue InstanceInstanceField = LuaValueNpcInstance.GetField("__instance");
	if (InstanceInstanceField.Type != ELuaValueType::String)
		return FLuaValue();

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	auto Location = FVector::ZeroVector;
	auto Rotation = FRotator::ZeroRotator;
	if (LuaValueSpawnPoint.Type == ELuaValueType::Table)
	{
		FLuaValue X = LuaValueSpawnPoint.GetField("x");
		if (X.Type == ELuaValueType::Integer || X.Type == ELuaValueType::Number)
			Location.X = X.ToFloat();

		FLuaValue Y = LuaValueSpawnPoint.GetField("y");
		if (Y.Type == ELuaValueType::Integer || Y.Type == ELuaValueType::Number)
			Location.Y = Y.ToFloat();

		FLuaValue Z = LuaValueSpawnPoint.GetField("z");
		if (Z.Type == ELuaValueType::Integer || Z.Type == ELuaValueType::Number)
			Location.Z = Z.ToFloat();
	}
	else if (LuaValueSpawnPoint.Type == ELuaValueType::String)
	{
		FString WaypointName = LuaValueSpawnPoint.ToString();
		auto Waypoint = GameMode->WaypointMap.Find(WaypointName);
		if (Waypoint)
		{
			Location = Waypoint->Position;
			Rotation = Waypoint->Direction;
		}
	}

	if (!NpcClass)
		return FLuaValue();

	FActorSpawnParameters SpawnInfo;
	FTransform SpawnTransform(Rotation,Location, FVector(1,1,1));
	auto Npc = World->SpawnActorDeferred<AGHNPC>(NpcClass, SpawnTransform);
	if (Npc)
	{
		Npc->_table = CreateLuaObject(LuaValueNpcInstance);
		// Npc->ActionQueue->NPC = Npc;
		if (InstanceInstanceField.ToString().ToUpper() == "PC_HERO")
			Npc->IsPlayer = true;

		UE_LOG(Lua, Warning, TEXT("----------------------"));
		auto LuaValueKeyArray = ULuaBlueprintFunctionLibrary::LuaTableGetKeys(Npc->_table);
		auto LuaValueValueArray = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Npc->_table);
		if (LuaValueKeyArray.Num() == LuaValueValueArray.Num())
		{
			for (int32 i = 0; i < LuaValueKeyArray.Num(); i++)
			{
				UE_LOG(Lua, Warning, TEXT("NPC, key: %s, value: %s"),
					*LuaValueKeyArray[i].ToString(), *LuaValueValueArray[i].ToString());
			}
		}

		auto LuaValueDailyRoutine = LuaValueNpcInstance.GetField("daily_routine");
		if (LuaValueDailyRoutine.Type == ELuaValueType::Function)
		{
			auto LuaValueOldSelf = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, StaticClass(), "self");
			ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, StaticClass(), "self", FLuaValue(Npc));

			ULuaBlueprintFunctionLibrary::LuaGlobalCallValue(World, StaticClass(), LuaValueDailyRoutine, {});
	
			ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, StaticClass(), "self", LuaValueOldSelf);
		}
		else
		{
			UE_LOG(Lua, Warning, TEXT("can't find daily_routine"));
		}
		
		// before?
		Npc->FinishSpawning(SpawnTransform);
		GameMode->NpcArray.Add(Npc);
		
		const auto LuaValueInit = LuaValueNpcInstance.GetField("__init");
		if (LuaValueInit.Type == ELuaValueType::Function)
		{
			auto LuaValueOldSelf = ULuaBlueprintFunctionLibrary::LuaGetGlobal(World, StaticClass(), "self");
			ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, StaticClass(), "self", FLuaValue(Npc));

			ULuaBlueprintFunctionLibrary::LuaGlobalCallValue(World, StaticClass(), LuaValueInit, {});
	
			ULuaBlueprintFunctionLibrary::LuaSetGlobal(World, StaticClass(), "self", LuaValueOldSelf);
		}



		// after? // crush crush ((((
		// Npc->FinishSpawning(SpawnTransform);
		// GameMode->NpcArray.Add(Npc);
		// ai possess in begin play
		
		return FLuaValue(Npc);
	}

	return FLuaValue();
	// auto SpawnedActor = World->SpawnActor(NpcClass, &Location, &Rotation, SpawnInfo);
	// if (!IsValid(SpawnedActor))
	// 	return;
	//
	// auto Npc = Cast<AGHNPC>(SpawnedActor);
	// if (Npc)
	// {
	// 	Npc->_table = NpcInstance;
	// 	
	// 	if (GameMode)
	// 		GameMode->NpcArray.Add(Npc);
	// }
}

// Checks if MOBSI object is available, in 10 meters around npc. Returns TRUE, if MOBSI has a free slot or npc already connected to this MOBSI.
// int Wld_IsMobAvailable(C_Npc npc, string schemeName)
FLuaValue UGHLuaState::Wld_IsMobAvailable(FLuaValue LuaValueNpc, FLuaValue Schemename)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_IsMobAvailable");

	return FLuaValue();
}

// Return current state-id of mob, detected by "npc".
// int Wld_GetMobState(C_Npc self, string schemeName)
FLuaValue UGHLuaState::Wld_GetMobState(FLuaValue Self, FLuaValue Schemename)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_GetMobState");

	return FLuaValue();
}

// Returns TRUE, if nearest to 'npc' free point, with name 'fpName' is available.
// If point is locked by npc - also returns TRUE.
// int Wld_IsFPAvailable(C_Npc npc, string fpName)
FLuaValue UGHLuaState::Wld_IsFPAvailable(FLuaValue LuaValueNpc, FLuaValue Fpname)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_IsFPAvailable");

	return FLuaValue();
}

// Same as Wld_IsFPAvailable, but current waypoint is going to be excluded from search.
// int Wld_IsNextFPAvailable(C_Npc npc, string fpName)
FLuaValue UGHLuaState::Wld_IsNextFPAvailable(FLuaValue LuaValueNpc, FLuaValue Fpname)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_IsNextFPAvailable");

	return FLuaValue();
}

// Inserts an C_Item object in world, at ‘spawnPoint’ location.
// void Wld_InsertItem(int itemInstance, string spawnPoint)
void UGHLuaState::Wld_InsertItem(FLuaValue ItemInstance, FLuaValue SpawnPoint)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Wld_InsertItem");

	if (ItemInstance.Type != ELuaValueType::Table)
		return;
	
	FLuaValue InstanceTypeField = ItemInstance.GetField("__type");
	if (InstanceTypeField.Type != ELuaValueType::String)
		return;

	if (InstanceTypeField.ToString() != "C_Item")
		return;

	FLuaValue InstanceInstanceField = ItemInstance.GetField("__instance");
	if (InstanceInstanceField.Type != ELuaValueType::String)
		return;

	auto World = GEngine->GetCurrentPlayWorld();
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	FVector Location = FVector::ZeroVector;
	if (SpawnPoint.Type == ELuaValueType::Table)
	{
		FLuaValue X = SpawnPoint.GetField("x");
		if (X.Type == ELuaValueType::Integer || X.Type == ELuaValueType::Number)
			Location.X = X.ToFloat();

		FLuaValue Y = SpawnPoint.GetField("y");
		if (Y.Type == ELuaValueType::Integer || Y.Type == ELuaValueType::Number)
			Location.Y = Y.ToFloat();

		FLuaValue Z = SpawnPoint.GetField("z");
		if (Z.Type == ELuaValueType::Integer || Z.Type == ELuaValueType::Number)
			Location.Z = Z.ToFloat();
	}
	else if (SpawnPoint.Type == ELuaValueType::String)
	{
		FString WaypointName = SpawnPoint.ToString();
		auto Waypoint = GameMode->WaypointMap.Find(WaypointName);
		if (Waypoint)
			Location = Waypoint->Position;
	}

	FRotator Rotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnInfo;

	auto SpawnedActor = World->SpawnActor(EntityClass, &Location, &Rotation, SpawnInfo);
	auto Item = Cast<AGHEntity>(SpawnedActor);
	if (Item)
	{
		Item->_table = ItemInstance;
	}
}


// Assign a new attitude value for pair of guilds.
// void Wld_SetGuildAttitude(int guild1, int attitude, int guild2)
void UGHLuaState::Wld_SetGuildAttitude(FLuaValue Guild1, FLuaValue Attitude, FLuaValue Guild2)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_SetGuildAttitude");

}

// Return a attitude betwen 'guild1' and 'guild2'.
// int Wld_GetGuildAttitude(int guild1, int guild2)
FLuaValue UGHLuaState::Wld_GetGuildAttitude(FLuaValue Guild1, FLuaValue Guild2)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_GetGuildAttitude");

	return FLuaValue();
}

// Detects an object of class "C_Npc" around 'npc' with specified  properties:
// "dest.instance" must be equal to "instance", if param "instance" is not -1
// "dest.aiState" must be equal to "aiState", if param "aiState" is not NOFUNC
// "dest.guild" must be equal to "guild", if param "guild" is not -1
// If such "dest" exist, then function returns TRUE and write result to global variable "other".
// int Wld_DetectNpc(C_Npc npc, int instance, func aiState, int guild)
FLuaValue UGHLuaState::Wld_DetectNpc(FLuaValue LuaValueNpc, FLuaValue Instance, FLuaValue Aistate, FLuaValue Guild)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_DetectNpc");

	return FLuaValue();
}

// Same as Wld_DetectNpc, but excludes player from search, if "detectPlayer"!=TRUE.
// int Wld_DetectNpcEx(C_Npc npc, int npcInstance, func aiState, int guild, int detectPlayer)
FLuaValue UGHLuaState::Wld_DetectNpcEx(FLuaValue LuaValueNpc, FLuaValue LuaValueNpcinstance, FLuaValue Aistate, FLuaValue Guild, FLuaValue Detectplayer)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_DetectNpcEx");

	return FLuaValue();
}

// Returns TRUE, if item described by flags can be found nearby npc
// int Wld_DetectItem(C_Npc npc, int flags)
FLuaValue UGHLuaState::Wld_DetectItem(FLuaValue LuaValueNpc, FLuaValue Flags)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_DetectItem");

	return FLuaValue();
}

// assigns a new 'guildId' to room, specified by 'room'
// void Wld_AssignRoomToGuild(string room, int guild)
void UGHLuaState::Wld_AssignRoomToGuild(FLuaValue Room, FLuaValue Guild)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_AssignRoomToGuild");

}

// if 'player' located inside room, return guild assigned for this room.
// otherwise returns GIL_NONE
// same as "Npc_GetPortalGuild(player)"
// int Wld_GetPlayerPortalGuild()
FLuaValue UGHLuaState::Wld_GetPlayerPortalGuild()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_GetPlayerPortalGuild");

	return FLuaValue();
}

// Return guild of portal from where player come from.
// int Wld_GetFormerPlayerPortalGuild()
FLuaValue UGHLuaState::Wld_GetFormerPlayerPortalGuild()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_GetFormerPlayerPortalGuild");

	return FLuaValue();
}

// Spawn "count" npc of certain type, around "npc".
// void Wld_SpawnNpcRange(C_Npc npc, int clsId, int count, int lifetime)
void UGHLuaState::Wld_SpawnNpcRange(FLuaValue LuaValueNpc, FLuaValue Clsid, FLuaValue Count, FLuaValue Lifetime)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Wld_SpawnNpcRange");

}

// Override an animation set of 'npc' with animation-set 'set'
// Example</li>
// </ul>
// <div class="highlight highlight-source-c++ notranslate position-relative overflow-auto"><pre><span class="pl-en">Mdl_ApplyOverlayMds</span>(self,<span class="pl-s"><span class="pl-pds">"</span>HUMANS_FLEE.MDS<span class="pl-pds">"</span></span>); <span class="pl-c"><span class="pl-c">//</span> overrides run animation, to run very-very fast</span></pre><div class="zeroclipboard-container position-absolute right-0 top-0">
//     <clipboard-copy aria-label="Copy" class="ClipboardButton btn js-clipboard-copy m-2 p-0 tooltipped-no-delay" data-copy-feedback="Copied!" data-tooltip-direction="w" value="Mdl_ApplyOverlayMds(self,&quot;HUMANS_FLEE.MDS&quot;); // overrides run animation, to run very-very fast" tabindex="0" role="button">
//       <svg aria-hidden="true" height="16" viewBox="0 0 16 16" version="1.1" width="16" data-view-component="true" class="octicon octicon-copy js-clipboard-copy-icon m-2">
//     <path d="M0 6.75C0 5.784.784 5 1.75 5h1.5a.75.75 0 0 1 0 1.5h-1.5a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 0 0 .25-.25v-1.5a.75.75 0 0 1 1.5 0v1.5A1.75 1.75 0 0 1 9.25 16h-7.5A1.75 1.75 0 0 1 0 14.25Z"></path><path d="M5 1.75C5 .784 5.784 0 6.75 0h7.5C15.216 0 16 .784 16 1.75v7.5A1.75 1.75 0 0 1 14.25 11h-7.5A1.75 1.75 0 0 1 5 9.25Zm1.75-.25a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 0 0 .25-.25v-7.5a.25.25 0 0 0-.25-.25Z"></path>
// </svg>
//       <svg aria-hidden="true" height="16" viewBox="0 0 16 16" version="1.1" width="16" data-view-component="true" class="octicon octicon-check js-clipboard-check-icon color-fg-success d-none m-2">
//     <path d="M13.78 4.22a.75.75 0 0 1 0 1.06l-7.25 7.25a.75.75 0 0 1-1.06 0L2.22 9.28a.751.751 0 0 1 .018-1.042.751.751 0 0 1 1.042-.018L6 10.94l6.72-6.72a.75.75 0 0 1 1.06 0Z"></path>
// </svg>
//     </clipboard-copy>
//   </div></div>
// <ul>
// <li>
// <p>"func void Mdl_RemoveOverlayMDS(var C_Npc npc, var string set)"
// Remove override-set, from npc.
// void Mdl_ApplyOverlayMds(C_Npc npc, string set)
void UGHLuaState::Mdl_ApplyOverlayMds(FLuaValue LuaValueNpc, FLuaValue Set)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Mdl_ApplyOverlayMds");

}

// Remove override-set, from npc.
// void Mdl_RemoveOverlayMDS(C_Npc npc, string set)
void UGHLuaState::Mdl_RemoveOverlayMDS(FLuaValue LuaValueNpc, FLuaValue Set)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Mdl_RemoveOverlayMDS");

}

// Override an animation set of 'npc' with animation-set 'set', for limited amout of time.
// timeTicks - time in miliseconds.
// Example
// void Mdl_ApplyOverlayMDSTimed(C_Npc npc, string set, float timeTicks)
void UGHLuaState::Mdl_ApplyOverlayMDSTimed(FLuaValue LuaValueNpc, FLuaValue Set, FLuaValue Timeticks)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Mdl_ApplyOverlayMDSTimed");

}

// Change scale of 'npc' view mesh.
// void Mdl_SetModelScale(C_Npc npc, float x, float y, float z)
void UGHLuaState::Mdl_SetModelScale(FLuaValue LuaValueNpc, FLuaValue X, FLuaValue Y, FLuaValue Z)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Mdl_SetModelScale");

}

// Set how "fat" this 'npc' is.
// Takes no effect in OpenGothic for now.
// void Mdl_SetModelFatness(C_Npc npc, float fatness)
void UGHLuaState::Mdl_SetModelFatness(FLuaValue LuaValueNpc, FLuaValue Fatness)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Mdl_SetModelFatness");

}

// Allocate new document object.
// Returns integer handle to created document.
// int Doc_Create()
FLuaValue UGHLuaState::Doc_Create()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_Create");

	return FLuaValue();
}

// Allocate new document/map object.
// Difference between 'Doc_Create' and 'Doc_CreateMap' is a cursor showing player position in world
// Returns integer handle to created document.
// int Doc_CreateMap()
FLuaValue UGHLuaState::Doc_CreateMap()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_CreateMap");

	return FLuaValue();
}

// Set pages count for document with handle 'document'
// void Doc_SetPages(int document, int count)
void UGHLuaState::Doc_SetPages(FLuaValue Document, FLuaValue Count)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_SetPages");

}

// Set background texture for 'page' of document
// If page is -1, then texture is going to be applyed to the document itself
// void Doc_SetPage(int document, int page, string texture, int scale)
void UGHLuaState::Doc_SetPage(FLuaValue Document, FLuaValue Page, FLuaValue Texture, FLuaValue Scale)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_SetPage");

}

// Set margins for 'page' of document
// If page is -1, then margins is going to be applyed to the document itself
// void Doc_SetMargins(int document, int page, int left, int top, int right, int bottom, int pixels)
void UGHLuaState::Doc_SetMargins(FLuaValue Document, FLuaValue Page, FLuaValue Left, FLuaValue Top, FLuaValue Right, FLuaValue Bottom, FLuaValue Pixels)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_SetMargins");

}

// Set font for 'page' of document
// If page is -1, then font is going to be applyed to the document itself. Example:
// void Doc_SetFont(int document, int page, string font)
void UGHLuaState::Doc_SetFont(FLuaValue Document, FLuaValue Page, FLuaValue Font)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_SetFont");

}

// Provide a world-file name as a hint to engine.
// Engine uses this information decide show or not player position on the map.
// void Doc_SetLevel(int document, string level)
void UGHLuaState::Doc_SetLevel(FLuaValue Document, FLuaValue Level)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_SetLevel");

}

// Provide a world size bounds as a hint to engine.
// Engine uses this information to display player position on the map.
// void Doc_SetLevelCoords(int document, int left, int top, int right, int bottom)
void UGHLuaState::Doc_SetLevelCoords(FLuaValue Document, FLuaValue Left, FLuaValue Top, FLuaValue Right, FLuaValue Bottom)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_SetLevelCoords");

}

// Append single line 'text' block to 'page' of the 'document'
// void Doc_PrintLine(int document, int page, string text)
void UGHLuaState::Doc_PrintLine(FLuaValue Document, FLuaValue Page, FLuaValue Text)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_PrintLine");

}

// Append multiline line 'text' block to 'page' of the 'document'
// void Doc_PrintLines(int document, int page, string text)
void UGHLuaState::Doc_PrintLines(FLuaValue Document, FLuaValue Page, FLuaValue Text)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_PrintLines");

}

// Present document with handle 'document' to screen.
// Handle is no longer valid after this call.
// void Doc_Show(int document)
void UGHLuaState::Doc_Show(FLuaValue Document)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Doc_Show");

}

// Add a new routine to 'npc' timetable. (start_h,start_m) - hour and minute to start; (stop_h,stop_m) - hour and minute to stop. 'state' - state function for this routine.
// if stop time less then start time, then routine going to be performed over midnight.
// void TA_Min(c_npc npc, int start_h, int start_m, int stop_h, int stop_m, func state, string waypoint)
void UGHLuaState::TA_Min(FLuaValue LuaValueNpc, FLuaValue LuaValueStartH, FLuaValue LuaValueStartM, FLuaValue LuaValueStopH, FLuaValue LuaValueStopM, FLuaValue LuaValueScriptStateName, FLuaValue LuaValueWaypoint)
{
	UE_LOG(Lua, Log, TEXT("TA_Min: LuaValueNpc: %s, LuaValueStartH: %s, LuaValueStartM %s,"
		" LuaValueStopH %s, LuaValueStopM %s, LuaValueScriptStateName %s, LuaValueWaypoint %s"),
		*LuaValueNpc.ToString(),
		*LuaValueStartH.ToString(), *LuaValueStartM.ToString(),
		*LuaValueStopH.ToString(), *LuaValueStopM.ToString(),
		*LuaValueScriptStateName.ToString(), *LuaValueWaypoint.ToString());
	
	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	if (!(LuaValueStartH.Type == ELuaValueType::Integer || LuaValueStartH.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueStartH.Type != ELuaValueType::Integer||Number"));
		return;
	}

	if (!(LuaValueStartM.Type == ELuaValueType::Integer || LuaValueStartM.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueStartM.Type != ELuaValueType::Integer||Number"));
		return;
	}

	if (!(LuaValueStopH.Type == ELuaValueType::Integer || LuaValueStopH.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueStopH.Type != ELuaValueType::Integer||Number"));
		return;
	}

	if (!(LuaValueStopM.Type == ELuaValueType::Integer || LuaValueStopM.Type == ELuaValueType::Number))
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueStopM.Type != ELuaValueType::Integer||Number"));
		return;
	}

	if (LuaValueScriptStateName.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueScriptStateName.Type != ELuaValueType::String"));
		return;
	}

	if (LuaValueWaypoint.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueWaypoint.Type != ELuaValueType::String"));
		return;
	}

	// TA_Min(self,start_h,start_m,stop_h,stop_m,ZS_Position,waypoint);
	// set wp

	auto NpcTable = Npc->_table;
	if (NpcTable.Type != ELuaValueType::Table)
	{
		UE_LOG(Lua, Warning, TEXT("NpcTable.Type != ELuaValueType::Table"));
		return;
	}

	NpcTable.SetField("wp", LuaValueWaypoint);
	
	auto DailyRoutine = NewObject<UGHNPCDailyRoutine>(Npc);
	// auto DailyRoutine = FDailyRoutine();
	DailyRoutine->StartHour = LuaValueStartH.ToInteger();
	DailyRoutine->StartMinute = LuaValueStartM.ToInteger();
	DailyRoutine->StopHour = LuaValueStopH.ToInteger();
	DailyRoutine->StopMinute = LuaValueStopM.ToInteger();
	DailyRoutine->ScriptStateName = LuaValueScriptStateName.ToString();
	DailyRoutine->WaypointName = LuaValueWaypoint.ToString();
	
	Npc->DailyRoutineArray.Add(DailyRoutine);
	
	UE_LOG(Lua, Log, TEXT("DailyRoutineArray.Add(DailyRoutine)"));
	
	//void UGHLuaState::AI_StartState(FLuaValue LuaValueNpc, FLuaValue StateFunctionName, FLuaValue IsFinalize, FLuaValue Waypoint)
}

// Shutdown game client
// void ExitGame()
void UGHLuaState::ExitGame()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::ExitGame");

}

// Shutdown game session
// void ExitSession()
void UGHLuaState::ExitSession()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::ExitSession");

}

// Returns FALSE, if video not played
// Note: opengothic returns always 1
// int PlayVideo(string filename)
FLuaValue UGHLuaState::PlayVideo(FLuaValue Filename)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::PlayVideo");

	return FLuaValue();
}

// Returns FALSE, if video not played.
// implemented as PlayVideo
// int PlayVideoEx(string filename, int screenBlend, int exitSession)
FLuaValue UGHLuaState::PlayVideoEx(FLuaValue Filename, FLuaValue Screenblend, FLuaValue Exitsession)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::PlayVideoEx");

	return FLuaValue();
}

// Shows chapter logo.
// title - Title text
// subtitle - subtitle text
// texture - background image (800x600 resolution recommended)
// sound - *.wav file to play
// waitTime - time to display chapter window, in milliseconds
// void IntroduceChapter(string title, string subtitle, string texture, string sound, int waitTime)
void UGHLuaState::IntroduceChapter(FLuaValue Title, FLuaValue Subtitle, FLuaValue Texture, FLuaValue Sound, FLuaValue WaitTime)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::IntroduceChapter");

	if (Title.Type != ELuaValueType::String || Subtitle.Type != ELuaValueType::String)
		return;

	if (Texture.Type != ELuaValueType::String || Sound.Type != ELuaValueType::String)
		return;

	if (!(WaitTime.Type == ELuaValueType::Integer || WaitTime.Type == ELuaValueType::Number))
		return;

	auto World = GEngine->GetCurrentPlayWorld();
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	GameMode->Chapter->ShowScreen(Title.ToString(), Subtitle.ToString(),
		Texture.ToString(), Sound.ToString(), WaitTime.ToFloat());
}

// Emit a global sound described by sound scheme(C_SFX) with name "scheme".
// void Snd_Play(string scheme)
void UGHLuaState::Snd_Play(FLuaValue Scheme)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Snd_Play");

}

// Emit a sound described by sound scheme(C_SFX) with name "scheme".
// Sound positioning in world is going to be attached to "npc" position.
// void Snd_Play3D(C_Npc npc, string scheme)
void UGHLuaState::Snd_Play3D(FLuaValue LuaValueNpc, FLuaValue Scheme)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Snd_Play3D");

}

// Return distance from 'npc' to closest sound source.
// int Snd_GetDistToSource(C_Npc npc)
FLuaValue UGHLuaState::Snd_GetDistToSource(FLuaValue LuaValueNpc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Snd_GetDistToSource");

	return FLuaValue();
}

// Writes "test" to debugoutput.
// void Game_InitGerman()
void UGHLuaState::Game_InitGerman()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Game_InitGerman");

}

// Writes "test" to debugoutput.
// void Game_InitEnglish()
void UGHLuaState::Game_InitEnglish()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "UGHLuaState::Game_InitEnglish");

}

// Writes "test" to debugoutput.
// void PrintDebugInst(string text)
void UGHLuaState::PrintDebugInst(FLuaValue Text)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::PrintDebugInst");

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Debug: " + Text.ToString());
}

// Writes "test" and "ch" to debugoutput.
// void PrintDebugInstCh(int ch, String text)
void UGHLuaState::PrintDebugInstCh(FLuaValue Ch, FLuaValue Text)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::PrintDebugInstCh");

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Debug: " + Text.ToString());
}

// Writes "test" and "ch" to debugoutput.
// void PrintDebugCh(int ch, String text)
void UGHLuaState::PrintDebugCh(FLuaValue Ch, FLuaValue Text)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::PrintDebugCh");

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Debug: " + Text.ToString());
}

void UGHLuaState::PrintDebug(FLuaValue Text)
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHLuaState::PrintDebug");

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Debug: " + Text.ToString());
}

// ---
// add more
// https://auronen.cokoliv.eu/gmc/pl/zengin/scripts/externals/mdl/
// ---

void UGHLuaState::Mdl_SetVisual(FLuaValue LuaValueNpc, FLuaValue LuaValueAnimationGroupName)
{
	UE_LOG(Lua, Log, TEXT("Mdl_SetVisual: LuaValueNpc: %s, LuaValueAnimationGroupName: %s"),
		*LuaValueNpc.ToString(), *LuaValueAnimationGroupName.ToString());
	
	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}

	if (LuaValueAnimationGroupName.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueAnimationGroupName.Type != ELuaValueType::String"));
		return;
	}

	Npc->SetVisual(LuaValueAnimationGroupName.ToString().ToUpper());
}

// var instance npc
// NPC to be affected
// var string body_mesh
// Mesh to be used as the body e.g. HUN_BODY_NAKED0
// var int body_tex
// Body texture assigned to this body mesh
// var int skin
// Body texture variant
// var string head_mesh
// Head mesh
// var int head_tex
// Head texture
// var int teeth_tex
// Teeth texture
// var int armor_inst
// Armor (C_ITEM instance) to be equipped or -1 for no armor
void UGHLuaState::Mdl_SetVisualBody(FLuaValue LuaValueNpc, FLuaValue LuaValueBodyMesh, FLuaValue LuaValueBodyTex,
	FLuaValue LuaValueSkin, FLuaValue LuaValueHeadMesh, FLuaValue LuaValueHeadTex,
	FLuaValue LuaValueTeethTex, FLuaValue LuaValueArmorInst)
{
	UE_LOG(Lua, Log, TEXT("Mdl_SetVisualBody: LuaValueNpc: %s, LuaValueBodyMesh: %s,"
		"LuaValueBodyTex: %s, LuaValueSkin: %s, LuaValueHeadMesh: %s,"
		"LuaValueHeadTex: %s, LuaValueTeethTex: %s, , LuaValueArmorInst: %s"),
		*LuaValueNpc.ToString(), *LuaValueBodyMesh.ToString(), *LuaValueBodyTex.ToString(),
		*LuaValueSkin.ToString(), *LuaValueHeadMesh.ToString(), *LuaValueHeadTex.ToString(),
		*LuaValueTeethTex.ToString() , *LuaValueArmorInst.ToString());
	
	auto Npc = Helpers::GetNpcFromLuaValue(LuaValueNpc);
	if (!Npc)
	{
		UE_LOG(Lua, Warning, TEXT("!Npc"));
		return;
	}
	
	if (LuaValueBodyMesh.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueBodyMesh.Type != ELuaValueType::String"));
	}
	else
	{
		auto BodyMesh = LuaValueBodyMesh.ToString();
		if (BodyMesh.Len() > 0)
		{
			if (LuaValueSkin.Type == ELuaValueType::Integer || LuaValueSkin.Type == ELuaValueType::Number)
			{
				// Npc->SetBodyMesh(BodyMesh, LuaValueSkin.ToInteger());
				Npc->SetBodyMesh(BodyMesh);
			}
			else
			{
				Npc->SetBodyMesh(BodyMesh);
			}
		}
	}
	
	if (LuaValueHeadMesh.Type != ELuaValueType::String)
	{
		UE_LOG(Lua, Warning, TEXT("LuaValueHeadMesh.Type != ELuaValueType::String"));
	}
	else
	{
		auto HeadMesh = LuaValueHeadMesh.ToString();
		if (HeadMesh.Len() > 0)
		{
			Npc->SetHeadMesh(HeadMesh);
		}
	}
	
	// @TODO: add armor
}

// Mdl_SetVisualBody(self,"hum_body_Naked0",0,2,"Hum_Head_Thief",15,4,stt_armor_h);