// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GNUReturnToMainMenu.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "GNUMultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "GameModes/GNUGameMode.h"
#include "Characters/GnuMyCharacter.h"
#include "Characters/GnuMyPlayerController.h"

void UGNUReturnToMainMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	//bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (ReturnButton)
	{
		ReturnButton->OnClicked.AddDynamic(this, &ThisClass::ReturnButtonClicked);
	}
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &ThisClass::RestartButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UGNUMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		}
	}
}

bool UGNUReturnToMainMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UGNUReturnToMainMenu::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		if (GameMode) // 서버에 있다면
		{
			GameMode->ReturnToMainMenuHost();
		}
		else // 클라이언트에 있다면
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UGNUReturnToMainMenu::MenuTearDown()
{
	RemoveFromParent();

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}



void UGNUReturnToMainMenu::ReturnButtonClicked()
{
	// 세션이 없는데 버튼을 계속 누르면 DestorySession 함수가 계속 호출 될 것이기에 비활성화
	ReturnButton->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UGNUReturnToMainMenu::RestartButtonClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
		AGnuMyPlayerController* GnuPlayerController = Cast<AGnuMyPlayerController>(GetOwningPlayer());
		if (GnuPlayerController)
		{
			AGnuMyCharacter* GnuCharacter = Cast<AGnuMyCharacter>(GnuPlayerController->GetPawn());
			if (GnuCharacter)
			{
				GnuCharacter->RestartingGame();
			}
		}
	}
}
