// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GNUGameMode.h"
#include "Characters/GnuCharacter.h"
#include "PlayerController/GNUPlayerController.h"
// #include "GnuMonster.h"

AGNUGameMode::AGNUGameMode()
{
	bDelayedStart = true;
}

void AGNUGameMode::PlayerEliminated(AGnuCharacter* ElimmedCharacter, AGNUPlayerController* VictimController, AGNUPlayerController* Monster)
{

}
