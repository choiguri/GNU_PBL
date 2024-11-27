// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GNUGameMode.h"
#include "Characters/GnuMyCharacter.h"
#include "Characters/GnuMyPlayerController.h"
#include "Monster/GnuMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

AGNUGameMode::AGNUGameMode()
{
	bDelayedStart = false;
}

void AGNUGameMode::PlayerEliminated(AGnuMyCharacter* ElimmedCharacter, AGnuMyPlayerController* VictimController, AController* Monster)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void AGNUGameMode::RequestRespawn(ACharacter* ElimedCharacter, AController* ElimedController)
{
	if (ElimedCharacter)
	{
		// 캐릭터와 컨트롤러 분리, 컨트롤러에 대한 소유권 호출
		ElimedCharacter->Reset();

		ElimedCharacter->Destroy();
	}
	if (ElimedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

		// 시작지점에서 부활
		RestartPlayerAtPlayerStart(ElimedController, PlayerStarts[Selection]);
	}
}
