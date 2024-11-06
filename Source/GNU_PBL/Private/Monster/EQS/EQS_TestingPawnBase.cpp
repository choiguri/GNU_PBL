// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/EQS/EQS_TestingPawnBase.h"

AEQS_TestingPawnBase::AEQS_TestingPawnBase()
{
	EQSTeamID = FGenericTeamId(1);

    if (GetGenericTeamId() == FGenericTeamId(1))
    {
        UE_LOG(LogTemp, Warning, TEXT("Team ID successfully set to 0."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to set Team ID to 0. Current Team ID: %d"), GetGenericTeamId().GetId());
    }
}

FGenericTeamId AEQS_TestingPawnBase::GetGenericTeamId() const
{
	return EQSTeamID;
}
