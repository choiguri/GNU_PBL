// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GNUGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGNUGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGNUGameMode();
	virtual void PlayerEliminated(class AGnuMyCharacter* ElimmedCharacter, class AGnuMyPlayerController* VictimController, AController* Monster);

	virtual void RequestRespawn(ACharacter* ElimedCharacter, AController* ElimedController);

	UPROPERTY()
	int32 TotalDeath = 0;

	void UpdateTotalDeath();

	bool bOverDeathCount();

	int32 GetTotalDeath();

	virtual void RestartGame() override;

	TArray<FString> GetPlayersName();
	TArray<float> GetPlayersHealth();

	void MonsterEliminated();

	/*bool bIsTimeOver();*/

};
