// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuCharacterController.h"

AGnuCharacterController::AGnuCharacterController()
{
	CharacterTeamID = FGenericTeamId(0);

    if (GetGenericTeamId() == FGenericTeamId(0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Team ID successfully set to 0."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to set Team ID to 0. Current Team ID: %d"), GetGenericTeamId().GetId());
    }
}

FGenericTeamId AGnuCharacterController::GetGenericTeamId() const
{
	return CharacterTeamID;
}
