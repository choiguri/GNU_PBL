// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/GNULobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void AGNULobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (GameState)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow,
				FString::Printf(TEXT("Players in Game: %d"), NumberOfPlayers));
		}
	}
	APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();

		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Cyan,
			FString::Printf(TEXT("%s has joined the game"), *PlayerName));

	}

	// 플레이어가 n명일 때 GameMap으로 이동 
	if (NumberOfPlayers == 2) // 나중에 수정 필요
	{
		bUseSeamlessTravel = true;
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/GNU/Maps/GameMap?listen"));
		}
	}
}

void AGNULobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// 플레이어가 game에서 나갔을 때
	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow,
			FString::Printf(TEXT("Players in Game: %d"), NumberOfPlayers - 1));

		FString PlayerName = PlayerState->GetPlayerName();

		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Cyan,
			FString::Printf(TEXT("%s has exited the game"), *PlayerName));
	}
}
