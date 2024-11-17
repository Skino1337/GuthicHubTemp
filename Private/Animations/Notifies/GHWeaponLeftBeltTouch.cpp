// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notifies/GHWeaponLeftBeltTouch.h"

#include "Game/GHAnimInstance.h"
#include "Game/GHNPC.h"


void UGHWeaponLeftBeltTouch::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	auto NotifyName = GetNotifyName();
	auto Owner = MeshComp->GetOwner();
	if (Owner)
	{
		auto Npc = Cast<AGHNPC>(Owner);
		if (Npc)
		{
			// if (Npc->AnimInstance->CurrentMontageName.Contains("WithdrawingSword"))
			// {
			// 	Npc->SwapMeshWeaponLeftBelt(true);
			// }
			// else if (Npc->AnimInstance->CurrentMontageName.Contains("SheathingSword"))
			// {
			// 	Npc->SwapMeshWeaponLeftBelt(false);
			// }
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Notify: " + NotifyName);
}