// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GNUHUD.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGNUHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	class UGNUCharacterOverlay* CharacterOverlay;

	/*UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverHeadClass;

	class UGNUOverHeadWidget* CharacterOverHead;*/

protected:
	virtual void BeginPlay() override;

	void AddCharacterOverlay();

	/*void AddCharacterOverHead();*/
};
