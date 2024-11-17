// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHAnimNotify.h"
#include "Game/GHNPC.h"


void UGHAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	auto NotifyName = GetNotifyName();
	auto Owner = MeshComp->GetOwner();
	if (Owner)
	{
		auto Npc = Cast<AGHNPC>(Owner);
		if (Npc)
		{
			// Npc->TakeWeaponMesh();
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Notify: " + NotifyName);
}