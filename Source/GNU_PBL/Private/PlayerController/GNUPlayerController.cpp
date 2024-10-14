// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/GNUPlayerController.h"
#include "HUD/GNUHUD.h"
#include "HUD/GNUCharacterOverlay.h"
#include "HUD/GNUOverHeadWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void AGNUPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GNUHUD = Cast<AGNUHUD>(GetHUD());
}

void AGNUPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	bool bHUDValid = GNUHUD && 
		GNUHUD->CharacterOverlay && 
		GNUHUD->CharacterOverlay->HealthBar && 
		GNUHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		GNUHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		GNUHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AGNUPlayerController::SetHUDPlayerName(APawn* InPawn)
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	if (GNUHUD && GNUHUD->CharacterOverHead && GNUHUD->CharacterOverHead->DisplayText)
	{
		APlayerState* PlayerStates = InPawn->GetPlayerState();

		FString PlayerName = "";
		if (PlayerStates)
		{
			PlayerName = PlayerStates->GetPlayerName();
		}
		GNUHUD->CharacterOverHead->DisplayText->SetText(FText::FromString(PlayerName));
	}

}
