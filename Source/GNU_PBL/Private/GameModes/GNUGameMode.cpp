// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GNUGameMode.h"
#include "Characters/GnuMyCharacter.h"
#include "Characters/GnuMyPlayerController.h"
// #include "GnuMonster.h"

AGNUGameMode::AGNUGameMode()
{
	bDelayedStart = true;
}

void AGNUGameMode::PlayerEliminated(AGnuCharacter* ElimmedCharacter, AGNUPlayerController* VictimController, AGNUPlayerController* Monster)
{

}
