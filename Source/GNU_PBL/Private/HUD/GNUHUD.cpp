// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GNUHUD.h"
#include "GameFramework/PlayerController.h"
#include "HUD/GNUOverHeadWidget.h"
#include "HUD/GNUCharacterOverlay.h"


void AGNUHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
	/*AddCharacterOverHead();*/
}

void AGNUHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UGNUCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
		
	}
}

//void AGNUHUD::AddCharacterOverHead()
//{
//	APlayerController* PlayerController = GetOwningPlayerController();
//	if (PlayerController && CharacterOverHeadClass)
//	{
//		CharacterOverHead = CreateWidget<UGNUOverHeadWidget>(PlayerController, CharacterOverHeadClass);
//		CharacterOverHead->AddToViewport();
//	}
//}


void AGNUHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);


	}
}


