// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GnuPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_Death)
	int32 Death;

	UFUNCTION()
	virtual void OnRep_Death();

	void AddToDeath(int32 DeathAmount);
private:
	UPROPERTY()
	class AGnuMyCharacter* Character;
	UPROPERTY()
	class AGnuMyPlayerController* Controller;

	

	
};
