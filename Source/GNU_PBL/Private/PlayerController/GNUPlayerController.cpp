// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/GNUPlayerController.h"
#include "HUD/GNUHUD.h"
#include "HUD/GNUCharacterOverlay.h"
#include "HUD/GNUOverHeadWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "HUD/GNUReturnToMainMenu.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"




void AGNUPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GNUHUD = Cast<AGNUHUD>(GetHUD());

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (Subsystem)
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AGNUPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);

	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		
	}
	
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

void AGNUPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &AGNUPlayerController::ShowReturnToMainMenu);
	}
	/*InputComponent->BindAction("MainMenu", EInputEvent::IE_Pressed, this, &AGNUPlayerController::ShowReturnToMainMenu);*/
}
	
	

void AGNUPlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UGNUReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}

	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}


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

void AGNUPlayerController::HighPingWarning()
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	bool bHUDValid = GNUHUD &&
		GNUHUD->CharacterOverlay &&
		GNUHUD->CharacterOverlay->HighPingImage &&
		GNUHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		GNUHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		GNUHUD->CharacterOverlay->PlayAnimation(GNUHUD->CharacterOverlay->HighPingAnimation);
	}
}

void AGNUPlayerController::StopHighPingWarning()
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	bool bHUDValid = GNUHUD &&
		GNUHUD->CharacterOverlay &&
		GNUHUD->CharacterOverlay->HighPingImage &&
		GNUHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		GNUHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (GNUHUD->CharacterOverlay->IsAnimationPlaying(GNUHUD->CharacterOverlay->HighPingAnimation))
		{
			GNUHUD->CharacterOverlay->StopAnimation(GNUHUD->CharacterOverlay->HighPingAnimation);
		}
		
	}
}

// 서버와 클라이언트 사이의 RoundTripTime 계산 하기 위한 함수들
// 클라이언트가 요청하고 서버에 도착하는 시간, 서버에서 클라이언트로 응답하는 시간
// 두 개를 합쳐서 RTT, 하지만 두 개가 같지는 않아서 RTT / 2 가 각각의 시간이라고 장담하지는 못한다.
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

