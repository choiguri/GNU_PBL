// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GNUPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGNUPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDPlayerName(APawn* InPawn);

protected:
	virtual void BeginPlay() override;

private:

	class AGNUHUD* GNUHUD;

};
