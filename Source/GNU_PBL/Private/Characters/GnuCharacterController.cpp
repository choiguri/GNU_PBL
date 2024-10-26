// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuCharacterController.h"

AGnuCharacterController::AGnuCharacterController()
{
	CharacterTeamID = FGenericTeamId(0);

}

FGenericTeamId AGnuCharacterController::GetGenericTeamId() const
{
	return CharacterTeamID;
}
