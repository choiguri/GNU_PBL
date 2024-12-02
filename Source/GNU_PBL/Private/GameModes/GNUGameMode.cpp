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
	if (ElimedCharacter)
	{
		// 캐릭터와 컨트롤러 분리, 컨트롤러에 대한 소유권 호출
		ElimedCharacter->Reset();

		ElimedCharacter->Destroy();
	}

	// 플레이어가 지는 조건 = 모든 플레이어들의 Death 합 >= 18 ( 5데카 기준 3인이면 18 4인이면 24) && 드래곤 피 > 0 일 때
	// 플레이어가 이기는 조건 = 드래곤 피 <= 0 일 때 
	// 드래곤 피를 어떻게 들고 오느냐가 문제일듯
	if (bOverDeathCount())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("TotalDeath is Over")));
		}

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
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%d"), TotalDeath));
				}
			}
		}
	}
}

// TotalDeath >= MaxRespawn * PlayerNum + PlayerNum

bool AGNUGameMode::bOverDeathCount()
{
	return TotalDeath >= 18;
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

//void AGNUGameMode::GetPlayersHealthAndName()
//{
//	TArray<float> PlayerHealth;
//	TArray<FString> PlayerName;
//
//	int32 Index = 0;  // Manually track the index for PlayerName and PlayerHealth
//
//	// Loop through all player controllers
//	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
//	{
//		AGnuMyPlayerController* PlayerController = Cast<AGnuMyPlayerController>(*Iterator);
//		if (PlayerController)
//		{
//			AGnuMyCharacter* PlayerCharacter = Cast<AGnuMyCharacter>(PlayerController->GetPawn());
//			if (PlayerCharacter)
//			{
//				PlayerHealth.EmplaceAt(Index, PlayerCharacter->GetHealth());
//			}
//
//			AGnuPlayerState* CharacaterPlayerState = Cast<AGnuPlayerState>(PlayerController->PlayerState);
//			if (CharacaterPlayerState)
//			{
//				PlayerName.EmplaceAt(Index, CharacaterPlayerState->GetPlayerName());
//			}
//
//			//// Set the HUD health and name for each player using the manually tracked index
//			//PlayerController->SetHUDOthersHealth(*PlayerName[Index], PlayerHealth[Index], 100.f, Index);
//			//if (GEngine)
//			//{
//			//	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Black, FString::Printf(TEXT("Index : %d, PlayerName : %s, PlayerHealth : %.2f"), Index, *PlayerName[Index], PlayerHealth[Index]));
//			//}
//		}
//		Index++;
//	}
//}

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
		}
		Index++;
	}


	return PlayerHealth;
}

