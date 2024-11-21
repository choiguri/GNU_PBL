// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuReloadComplete.h"
#include "Characters/GnuMyCharacter.h"
#include "Weapons/CombatComponent.h"

void UGnuReloadComplete::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGnuMyCharacter* Character = Cast<AGnuMyCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		if (Character->Combat)
		{
			Character->Combat->ReloadFinished();
		}
	}
}
