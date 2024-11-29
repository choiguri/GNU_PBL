// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/GnuPlayerState.h"
#include "Characters/GnuMyCharacter.h"
#include "Characters/GnuMyPlayerController.h"
#include "Net/UnrealNetwork.h"


void AGnuPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGnuPlayerState, Death);
}

void AGnuPlayerState::AddToDeath(int32 DeathAmount)
{
	Death += DeathAmount;

	Character = Character == nullptr ? Cast<AGnuMyCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AGnuMyPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDDeathCount(Death);
		}
	}
}

void AGnuPlayerState::OnRep_Death()
{
	Character = Character == nullptr ? Cast<AGnuMyCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AGnuMyPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDDeathCount(Death);
		}
	}
}

