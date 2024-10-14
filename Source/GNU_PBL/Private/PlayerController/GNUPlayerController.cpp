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

void AGNUPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	
}

void AGNUPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
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

void AGNUPlayerController::SetHUDCombatTime(float CombatTime)
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	bool bHUDValid = GNUHUD &&
		GNUHUD->CharacterOverlay &&
		GNUHUD->CharacterOverlay->CombatTimeText;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CombatTime / 60.f);
		int32 Seconds = CombatTime - Minutes * 60;

		FString TimeText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		GNUHUD->CharacterOverlay->CombatTimeText->SetText(FText::FromString(TimeText));
	}
}



void AGNUPlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(TotalTime - GetServertime());

	if (CountdownInt != SecondsLeft)
	{
		SetHUDCombatTime(TotalTime - GetServertime());
		
	}

	CountdownInt = SecondsLeft;
}

void AGNUPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;

	}
}

float AGNUPlayerController::GetServertime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AGNUPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AGNUPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

//void AGNUPlayerController::SetHUDPlayerName(APawn* InPawn)
//{
//	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;
//
//	if (GNUHUD && GNUHUD->CharacterOverHead && GNUHUD->CharacterOverHead->DisplayText)
//	{
//		APlayerState* PlayerStates = InPawn->GetPlayerState();
//
//		FString PlayerName = "";
//		if (PlayerStates)
//		{
//			PlayerName = PlayerStates->GetPlayerName();
//		}
//		GNUHUD->CharacterOverHead->DisplayText->SetText(FText::FromString(PlayerName));
//	}
//
//}
