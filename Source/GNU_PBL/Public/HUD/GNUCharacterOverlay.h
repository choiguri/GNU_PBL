// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GNUCharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGNUCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StaminaText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CombatTimeText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathAmount;


	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBarPlayer1;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBarPlayer2;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBarPlayer3;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBarPlayer4;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player1NameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player2NameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player3NameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Player4NameText;


};
