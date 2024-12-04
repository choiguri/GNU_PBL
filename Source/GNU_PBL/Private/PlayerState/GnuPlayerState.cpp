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

}

void AGnuPlayerState::UpdateDeathCountToHUD()
{
	Character = Character == nullptr ? Cast<AGnuMyCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AGnuMyPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			int32 DeathNum = FMath::Clamp(MaxRespawn - Death, 0, MaxRespawn);
			Controller->SetHUDDeathCount(DeathNum);
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
			int32 DeathNum = FMath::Clamp(MaxRespawn - Death, 0, MaxRespawn);
			Controller->SetHUDDeathCount(DeathNum);
		}
	}
}

