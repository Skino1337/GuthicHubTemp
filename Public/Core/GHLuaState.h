// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LuaState.h"
#include "GHLuaState.generated.h"

#define C_ITEM "C_Item"

DECLARE_LOG_CATEGORY_EXTERN(Lua, Log, All);

UCLASS()
class GOTHICHUB_API UGHLuaState : public ULuaState
{
	GENERATED_BODY()
	
protected:
	void LuaStateInit() override;
	
public:
	UGHLuaState();

	UFUNCTION()
	static FLuaValue CreateLuaObject(FLuaValue Instance);
	
	// __index(object, key) -> returning 1 value
	LUACFUNCTION(UGHLuaState, MetaMethodIndex, 1, 2);
	
	// __newindex(object, key, value)
	LUACFUNCTION(UGHLuaState, MetaMethodNewIndex, 0, 3);
	
	// __eq(object1, object2) -> returning bool
	LUACFUNCTION(UGHLuaState, MetaMethodEq, 1, 2);
	
	// __string(object) -> returning string
	LUACFUNCTION(UGHLuaState, MetaMethodToString, 1, 1);

public:
	TSubclassOf<ACharacter> NpcClass;
	
	TSubclassOf<AActor> EntityClass;
	
	UFUNCTION()
	FLuaValue ConcatStrings(FLuaValue Lhs, FLuaValue Rhs);

	UFUNCTION()
	FLuaValue IntToString(FLuaValue X);

	UFUNCTION()
	FLuaValue FloatToString(FLuaValue X);

	UFUNCTION()
	FLuaValue Hlp_Random(FLuaValue Max);

	UFUNCTION()
	FLuaValue Hlp_StrCmp(FLuaValue S1, FLuaValue S2);

	UFUNCTION()
	FLuaValue Hlp_IsValidNpc(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Hlp_IsValidItem(FLuaValue Item);

	UFUNCTION()
	FLuaValue Hlp_IsItem(FLuaValue Item, FLuaValue ItemInstance);

	UFUNCTION()
	FLuaValue Hlp_GetNpc(FLuaValue LuaValueNpcInstance);

	UFUNCTION()
	FLuaValue Hlp_GetInstanceID(FLuaValue LuaValueNpc);

	// UFUNCTION()
	// FLuaValue Hlp_GetInstanceID(FLuaValue Item);

	UFUNCTION()
	FLuaValue Npc_GetStateTime(FLuaValue LuaValueNpc);

	UFUNCTION()
	void Npc_SetStateTime(FLuaValue LuaValueNpc, FLuaValue Seconds);

	UFUNCTION()
	FLuaValue Npc_GetNearestWP(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetNextWP(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetBodyState(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_HasBodyFlag(FLuaValue LuaValueNpc, FLuaValue Bodyflag);

	UFUNCTION()
	void Npc_SetToFistMode(FLuaValue LuaValueNpc);

	UFUNCTION()
	void Npc_SetToFightMode(FLuaValue LuaValueNpc, FLuaValue Weaponid);

	UFUNCTION()
	FLuaValue Npc_IsInFightMode(FLuaValue LuaValueNpc, FLuaValue LuaValueFightMode);

	UFUNCTION()
	FLuaValue Npc_GetReadiedWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_IsDrawingWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_HasReadiedMeleeWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetTarget(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetNextTarget(FLuaValue LuaValueNpc);

	UFUNCTION()
	void Npc_SetTarget(FLuaValue LuaValueNpc, FLuaValue Other);

	UFUNCTION()
	FLuaValue Npc_IsOnFP(FLuaValue LuaValueNpc, FLuaValue Fpname);

	UFUNCTION()
	FLuaValue Npc_GetInvItem(FLuaValue LuaValueNpc, FLuaValue Iteminstance);

	UFUNCTION()
	FLuaValue Npc_HasItems(FLuaValue LuaValueNpc, FLuaValue LuaValueItemInstance);

	UFUNCTION()
	void Npc_RemoveInvItem(FLuaValue LuaValueNpc, FLuaValue Iteminstance);

	UFUNCTION()
	void Npc_RemoveInvItems(FLuaValue LuaValueNpc, FLuaValue Iteminstance, FLuaValue Amount);

	UFUNCTION()
	FLuaValue Npc_ClearInventory(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetEquippedMeleeWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetEquippedRangedWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetEquippedArmor(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_HasEquippedWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_HasEquippedMeleeWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_HasEquippedRangedWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_HasEquippedArmor(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_OwnedByNpc(FLuaValue Item, FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_IsDetectedMobOwnedByNpc(FLuaValue LuaValueNpc, FLuaValue User);

	UFUNCTION()
	void Npc_SetRefuseTalk(FLuaValue LuaValueNpc, FLuaValue Timesec);

	UFUNCTION()
	FLuaValue Npc_RefuseTalk(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_IsDead(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_KnowsInfo(FLuaValue LuaValueNpc, FLuaValue Infoid);

	UFUNCTION()
	FLuaValue Npc_CheckInfo(FLuaValue LuaValueNpc, FLuaValue Important);

	UFUNCTION()
	FLuaValue Npc_IsPlayer(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetDistToNpc(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2);

	UFUNCTION()
	FLuaValue Npc_GetHeightToNpc(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2);

	UFUNCTION()
	FLuaValue Npc_GetHeightToItem(FLuaValue LuaValueNpc1, FLuaValue Itm);

	UFUNCTION()
	FLuaValue Npc_GetDistToWP(FLuaValue LuaValueNpc, FLuaValue LuaValueWaypointName);

	UFUNCTION()
	FLuaValue Npc_SetTrueGuild(FLuaValue LuaValueNpc, FLuaValue LuaValueGuild);

	UFUNCTION()
	FLuaValue Npc_GetTrueGuild(FLuaValue LuaValueNpc);

	UFUNCTION()
	void Npc_SetAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueAttitude);

	UFUNCTION()
	FLuaValue Npc_GetPermAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueOther);

	UFUNCTION()
	void Npc_SetTempAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueAttitude);

	UFUNCTION()
	FLuaValue Npc_GetAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueOther);

	UFUNCTION()
	FLuaValue Npc_GetGuildAttitude(FLuaValue LuaValueNpc, FLuaValue LuaValueOther);

	UFUNCTION()
	void Npc_ClearAIQueue(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_IsInState(FLuaValue LuaValueNpc, FLuaValue StateName);

	UFUNCTION()
	FLuaValue Npc_WasInState(FLuaValue LuaValueNpc, FLuaValue StateName);

	UFUNCTION()
	void Npc_ExchangeRoutine(FLuaValue LuaValueNpc, FLuaValue Routinename);

	UFUNCTION()
	void Npc_ChangeAttribute(FLuaValue LuaValueNpc, FLuaValue Atr, FLuaValue Value);

	UFUNCTION()
	FLuaValue Npc_GetLastHitSpellID(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetLastHitSpellCat(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_SetActiveSpellInfo(FLuaValue LuaValueNpc, FLuaValue I1);

	UFUNCTION()
	FLuaValue Npc_GetActiveSpellCat(FLuaValue LuaValueNpc);

	UFUNCTION()
	void Npc_PercEnable(FLuaValue LuaValueNpc, FLuaValue Percid, FLuaValue Callback);

	UFUNCTION()
	void Npc_PercDisable(FLuaValue LuaValueNpc, FLuaValue Percid);

	UFUNCTION()
	void Npc_SetPercTime(FLuaValue LuaValueNpc, FLuaValue Seconds);

	UFUNCTION()
	void Npc_SendPassivePerc(FLuaValue LuaValueNpc, FLuaValue Percid, FLuaValue Victum, FLuaValue Other);

	UFUNCTION()
	void Npc_PerceiveAll(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_GetDetectedMob(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_CanSeeNpc(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2);

	UFUNCTION()
	FLuaValue Npc_CanSeeNpcFreeLOS(FLuaValue LuaValueNpc1, FLuaValue LuaValueNpc2);

	UFUNCTION()
	void Npc_PlayAni(FLuaValue LuaValueNpc, FLuaValue Anim);

	UFUNCTION()
	FLuaValue Npc_GetPortalGuild(FLuaValue LuaValueNpc);

	UFUNCTION()
	FLuaValue Npc_IsInPlayersRoom(FLuaValue LuaValueNpc);

	UFUNCTION()
	void Log_CreateTopic(FLuaValue Name, FLuaValue Section);

	UFUNCTION()
	void Log_SetTopicStatus(FLuaValue Name, FLuaValue Status);

	UFUNCTION()
	void Log_AddEntry(FLuaValue Topic, FLuaValue Entry);

	UFUNCTION()
	void AI_Wait(FLuaValue LuaValueNpc, FLuaValue S);

	UFUNCTION()
	void AI_PlayAni(FLuaValue LuaValueNpc, FLuaValue Anim);

	UFUNCTION()
	void AI_StandUp(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_StandUpQuick(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_LookAtNpc(FLuaValue LuaValueNpc, FLuaValue Other);

	UFUNCTION()
	void AI_StopLookAt(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_UseItem(FLuaValue LuaValueNpc, FLuaValue Iteminstance);

	UFUNCTION()
	void AI_UseItemToState(FLuaValue LuaValueNpc, FLuaValue Iteminstance, FLuaValue State);

	UFUNCTION()
	FLuaValue AI_UseMob(FLuaValue LuaValueNpc, FLuaValue Schemename, FLuaValue Targetstate);

	UFUNCTION()
	void AI_SetWalkmode(FLuaValue LuaValueNpc, FLuaValue LuaValueWalkMode);

	UFUNCTION()
	void AI_GotoWP(FLuaValue LuaValueNpc, FLuaValue LuaValueWaypointName);

	UFUNCTION()
	void AI_GotoFP(FLuaValue LuaValueNpc, FLuaValue Fpname);

	UFUNCTION()
	void AI_GotoNextFP(FLuaValue LuaValueNpc, FLuaValue Fpname);

	UFUNCTION()
	void AI_GotoNpc(FLuaValue LuaValueNpc, FLuaValue LuaValueOther);

	UFUNCTION()
	void AI_Teleport(FLuaValue LuaValueNpc, FLuaValue Waypoint);

	UFUNCTION()
	void AI_TurnToNpc(FLuaValue LuaValueNpc, FLuaValue Other);

	UFUNCTION()
	void AI_AlignToWP(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_Dodge(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_PlayAniBS(FLuaValue LuaValueNpc, FLuaValue Aniname, FLuaValue Bodystate);

	UFUNCTION()
	void AI_RemoveWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_DrawWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_ReadyMeleeWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_ReadyRangedWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_Attack(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_FinishingMove(FLuaValue LuaValueNpc, FLuaValue Other);

	UFUNCTION()
	void AI_EquipBestMeleeWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_EquipBestRangedWeapon(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_EquipBestArmor(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_UnequipWeapons(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_UnequipArmor(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_EquipArmor(FLuaValue Owner, FLuaValue Itemid);

	UFUNCTION()
	void AI_Output(FLuaValue LuaValueNpc, FLuaValue Target, FLuaValue OutputName);

	UFUNCTION()
	void AI_OutputSVM(FLuaValue LuaValueNpc, FLuaValue Target, FLuaValue Svmname);

	UFUNCTION()
	void AI_OutputSVM_Overlay(FLuaValue LuaValueNpc, FLuaValue Target, FLuaValue Svmname);

	UFUNCTION()
	void AI_StopProcessInfos(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_StartState(FLuaValue LuaValueNpc, FLuaValue StateFunctionName, FLuaValue IsFinalize, FLuaValue Waypoint);

	UFUNCTION()
	void AI_StopAim(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_SetNpcsToState(FLuaValue LuaValueNpc, FLuaValue Aistatefunc, FLuaValue Radius);

	UFUNCTION()
	void AI_ContinueRoutine(FLuaValue LuaValueNpc);

	UFUNCTION()
	void AI_ReadySpell(FLuaValue LuaValueNpc, FLuaValue Spellid, FLuaValue Investmana);

	UFUNCTION()
	FLuaValue CreateInvItem(FLuaValue& LuaValueNpc, FLuaValue& LuaValueItemInstance);

	UFUNCTION()
	FLuaValue CreateInvItems(FLuaValue& LuaValueNpc, FLuaValue& LuaValueItemInstance, FLuaValue& LuaValueNumber);

	UFUNCTION()
	void EquipItem(FLuaValue LuaValueNpc, FLuaValue LuaValueItemInstance);

	UFUNCTION()
	FLuaValue Mob_HasItems(FLuaValue Mobname, FLuaValue Itemid);

	UFUNCTION()
	FLuaValue Wld_GetDay();

	UFUNCTION()
	FLuaValue Wld_IsTime(FLuaValue Hours, FLuaValue Mins, FLuaValue Houre, FLuaValue Mine);

	UFUNCTION()
	FLuaValue Wld_InsertNpc(FLuaValue LuaValueNpcInstance, FLuaValue LuaValueSpawnPoint);

	UFUNCTION()
	FLuaValue Wld_IsMobAvailable(FLuaValue LuaValueNpc, FLuaValue Schemename);

	UFUNCTION()
	FLuaValue Wld_GetMobState(FLuaValue Self, FLuaValue Schemename);

	UFUNCTION()
	FLuaValue Wld_IsFPAvailable(FLuaValue LuaValueNpc, FLuaValue Fpname);

	UFUNCTION()
	FLuaValue Wld_IsNextFPAvailable(FLuaValue LuaValueNpc, FLuaValue Fpname);

	UFUNCTION()
	void Wld_InsertItem(FLuaValue ItemInstance, FLuaValue SpawnPoint);

	UFUNCTION()
	void Wld_SetGuildAttitude(FLuaValue Guild1, FLuaValue Attitude, FLuaValue Guild2);

	UFUNCTION()
	FLuaValue Wld_GetGuildAttitude(FLuaValue Guild1, FLuaValue Guild2);

	UFUNCTION()
	FLuaValue Wld_DetectNpc(FLuaValue LuaValueNpc, FLuaValue Instance, FLuaValue Aistate, FLuaValue Guild);

	UFUNCTION()
	FLuaValue Wld_DetectNpcEx(FLuaValue LuaValueNpc, FLuaValue LuaValueNpcinstance, FLuaValue Aistate, FLuaValue Guild, FLuaValue Detectplayer);

	UFUNCTION()
	FLuaValue Wld_DetectItem(FLuaValue LuaValueNpc, FLuaValue Flags);

	UFUNCTION()
	void Wld_AssignRoomToGuild(FLuaValue Room, FLuaValue Guild);

	UFUNCTION()
	FLuaValue Wld_GetPlayerPortalGuild();

	UFUNCTION()
	FLuaValue Wld_GetFormerPlayerPortalGuild();

	UFUNCTION()
	void Wld_SpawnNpcRange(FLuaValue LuaValueNpc, FLuaValue Clsid, FLuaValue Count, FLuaValue Lifetime);

	UFUNCTION()
	void Mdl_ApplyOverlayMds(FLuaValue LuaValueNpc, FLuaValue Set);

	UFUNCTION()
	void Mdl_RemoveOverlayMDS(FLuaValue LuaValueNpc, FLuaValue Set);

	UFUNCTION()
	void Mdl_ApplyOverlayMDSTimed(FLuaValue LuaValueNpc, FLuaValue Set, FLuaValue Timeticks);

	UFUNCTION()
	void Mdl_SetModelScale(FLuaValue LuaValueNpc, FLuaValue X, FLuaValue Y, FLuaValue Z);

	UFUNCTION()
	void Mdl_SetModelFatness(FLuaValue LuaValueNpc, FLuaValue Fatness);

	UFUNCTION()
	FLuaValue Doc_Create();

	UFUNCTION()
	FLuaValue Doc_CreateMap();

	UFUNCTION()
	void Doc_SetPages(FLuaValue Document, FLuaValue Count);

	UFUNCTION()
	void Doc_SetPage(FLuaValue Document, FLuaValue Page, FLuaValue Texture, FLuaValue Scale);

	UFUNCTION()
	void Doc_SetMargins(FLuaValue Document, FLuaValue Page, FLuaValue Left, FLuaValue Top, FLuaValue Right, FLuaValue Bottom, FLuaValue Pixels);

	UFUNCTION()
	void Doc_SetFont(FLuaValue Document, FLuaValue Page, FLuaValue Font);

	UFUNCTION()
	void Doc_SetLevel(FLuaValue Document, FLuaValue Level);

	UFUNCTION()
	void Doc_SetLevelCoords(FLuaValue Document, FLuaValue Left, FLuaValue Top, FLuaValue Right, FLuaValue Bottom);

	UFUNCTION()
	void Doc_PrintLine(FLuaValue Document, FLuaValue Page, FLuaValue Text);

	UFUNCTION()
	void Doc_PrintLines(FLuaValue Document, FLuaValue Page, FLuaValue Text);

	UFUNCTION()
	void Doc_Show(FLuaValue Document);

	UFUNCTION()
	void TA_Min(FLuaValue LuaValueNpc, FLuaValue LuaValueStartH, FLuaValue LuaValueStartM, FLuaValue LuaValueStopH, FLuaValue LuaValueStopM, FLuaValue LuaValueScriptStateName, FLuaValue LuaValueWaypoint);

	UFUNCTION()
	void ExitGame();

	UFUNCTION()
	void ExitSession();

	UFUNCTION()
	FLuaValue PlayVideo(FLuaValue Filename);

	UFUNCTION()
	FLuaValue PlayVideoEx(FLuaValue Filename, FLuaValue Screenblend, FLuaValue Exitsession);

	UFUNCTION()
	void IntroduceChapter(FLuaValue Title, FLuaValue Subtitle, FLuaValue Texture, FLuaValue Sound, FLuaValue WaitTime);

	UFUNCTION()
	void Snd_Play(FLuaValue Scheme);

	UFUNCTION()
	void Snd_Play3D(FLuaValue LuaValueNpc, FLuaValue Scheme);

	UFUNCTION()
	FLuaValue Snd_GetDistToSource(FLuaValue LuaValueNpc);

	UFUNCTION()
	void Game_InitGerman();

	UFUNCTION()
	void Game_InitEnglish();

	UFUNCTION()
	void PrintDebugInst(FLuaValue Text);

	UFUNCTION()
	void PrintDebugInstCh(FLuaValue Ch, FLuaValue Text);

	UFUNCTION()
	void PrintDebugCh(FLuaValue Ch, FLuaValue Text);

	// custom
	
	UFUNCTION()
	void PrintDebug(FLuaValue Text);

	UFUNCTION()
	void Mdl_SetVisual(FLuaValue LuaValueNpc, FLuaValue LuaValueAnimationGroupName);

	UFUNCTION()
	void Mdl_SetVisualBody(FLuaValue LuaValueNpc, FLuaValue LuaValueBodyMesh, FLuaValue LuaValueBodyTex,
	FLuaValue LuaValueSkin, FLuaValue LuaValueHeadMesh, FLuaValue LuaValueHeadTex,
	FLuaValue LuaValueTeethTex, FLuaValue LuaValueArmorInst);
};