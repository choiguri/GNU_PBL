// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuHitReactMontageComplete.h"
#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/GnuMyPlayerController.h"

void UGnuHitReactMontageComplete::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGnuMyCharacter* Character = Cast<AGnuMyCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		if (Character->GNUPlayerController)
		{
			Character->EnableInput(Character->GNUPlayerController);
		}
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}
