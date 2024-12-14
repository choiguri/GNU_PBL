// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GNUGameMode.h"
#include "Characters/GnuMyCharacter.h"
#include "Characters/GnuMyPlayerController.h"
#include "Monster/GnuMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerState/GnuPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"


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

	AGnuPlayerState* VictimPlayerState = VictimController ? Cast<AGnuPlayerState>(VictimController->PlayerState) : nullptr;

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDeath(1);		
		VictimPlayerState->UpdateDeathCountToHUD();
		UpdateTotalDeath();
	}
}

void AGNUGameMode::RequestRespawn(ACharacter* ElimedCharacter, AController* ElimedController)
{
	// 플레이어가 지는 조건 = 모든 플레이어들의 Death 합 >= 18 ( 5데카 기준 3인이면 18 4인이면 24) && 드래곤 피 > 0 일 때

	if (bOverDeathCount())
	{
		AGnuMyPlayerController* PlayerController = Cast<AGnuMyPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PlayerController)
		{
			AGnuMyCharacter* PlayerCharacter = Cast<AGnuMyCharacter>(PlayerController->GetPawn());
			if (PlayerCharacter)
			{
				PlayerCharacter->CreateDefeatResultWidget();
				return;
			}
		}
	}

	if (ElimedCharacter)
	{
		// 캐릭터와 컨트롤러 분리, 컨트롤러에 대한 소유권 호출
		ElimedCharacter->Reset();

		ElimedCharacter->Destroy();
	}


	if (ElimedController)
	{
		AGnuPlayerState* ElimedCharacterPlayerState = Cast<AGnuPlayerState>(ElimedController->GetPlayerState<AGnuPlayerState>());
		if (ElimedCharacterPlayerState)
		{
			if (ElimedCharacterPlayerState->Death > ElimedCharacterPlayerState->MaxRespawn)
			{
				return;
			}
		}
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

// 게임 내의 모든 플레이어들의 Death 횟수의 합
void AGNUGameMode::UpdateTotalDeath()
{
	TotalDeath = 0;

	for (TObjectPtr<APlayerState> PlayerState : GameState.Get()->PlayerArray)
	{
		if (PlayerState)
		{
			AGnuPlayerState* GnuPlayerState = Cast<AGnuPlayerState>(PlayerState);
			if (GnuPlayerState)
			{
				TotalDeath += GnuPlayerState->Death;
				/*if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%d"), TotalDeath));
				}*/
			}
		}
	}
}

// TotalDeath >= MaxRespawn * PlayerNum + PlayerNum

bool AGNUGameMode::bOverDeathCount()
{
	return TotalDeath >= 16;
}

int32 AGNUGameMode::GetTotalDeath()
{
	return TotalDeath;
}

void AGNUGameMode::RestartGame()
{
	TotalDeath = 0;
	for (TObjectPtr<APlayerState> PlayerState : GameState.Get()->PlayerArray)
	{
		if (PlayerState)
		{
			AGnuPlayerState* GnuPlayerState = Cast<AGnuPlayerState>(PlayerState);
			if (GnuPlayerState)
			{
				GnuPlayerState->Death = 0;
			}
		}
	}

	Super::RestartGame();
}

TArray<FString> AGNUGameMode::GetPlayersName()
{
	TArray<FString> PlayerName;
	int32 Index = 0;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGnuMyPlayerController* PlayerController = Cast<AGnuMyPlayerController>(*Iterator);
		if (PlayerController)
		{
			AGnuPlayerState* CharacaterPlayerState = Cast<AGnuPlayerState>(PlayerController->PlayerState);
			if (CharacaterPlayerState)
			{
				PlayerName.EmplaceAt(Index, CharacaterPlayerState->GetPlayerName());
			}
			else
			{
				PlayerName.EmplaceAt(Index, "");
			}
		}
		Index++;
	}

	return PlayerName;
}

TArray<float> AGNUGameMode::GetPlayersHealth()
{
	TArray<float> PlayerHealth;
	int32 Index = 0;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGnuMyPlayerController* PlayerController = Cast<AGnuMyPlayerController>(*Iterator);
		if (PlayerController)
		{
			AGnuMyCharacter* PlayerCharacter = Cast<AGnuMyCharacter>(PlayerController->GetPawn());
			if (PlayerCharacter)
			{
				PlayerHealth.EmplaceAt(Index, PlayerCharacter->GetHealth());
			}
			else
			{
				PlayerHealth.EmplaceAt(Index, 0.0f);
			}
		}
		Index++;
	}


	return PlayerHealth;
}

void AGNUGameMode::MonsterEliminated()
{
	AGnuMyPlayerController* PlayerController = Cast<AGnuMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		AGnuMyCharacter* PlayerCharacter = Cast<AGnuMyCharacter>(PlayerController->GetPawn());
		if (PlayerCharacter)
		{
			PlayerCharacter->CreateVictoryResultWidget();
		}
	}
}

//bool AGNUGameMode::bIsTimeOver()
//{
//	AGnuMyPlayerController* PlayerController = Cast<AGnuMyPlayerController>(GetWorld()->GetFirstPlayerController());
//	if (PlayerController)
//	{
//		uint32 SecondsLeft = FMath::CeilToInt(PlayerController->TotalTime - PlayerController->GetServertime());
//		return SecondsLeft <= 0;
//	}
//
//	return false;
//}

