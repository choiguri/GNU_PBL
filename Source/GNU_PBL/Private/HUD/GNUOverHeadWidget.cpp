// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GNUOverHeadWidget.h"
#include "Components/TextBlock.h"
#include "GameFrameWork/PlayerState.h"

void UGNUOverHeadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UGNUOverHeadWidget::ShowPlayerName(APawn* InPawn)
{
	APlayerState* PlayerState = InPawn->GetPlayerState();

	if (PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();

		FString LocalRoleString = FString::Printf(TEXT("%s"), *PlayerName);
		SetDisplayText(LocalRoleString);
	}
	/*APlayerState* PlayerState = InPawn->GetPlayerState();

	FString PlayerName;
	if (PlayerState)
	{
		PlayerName = PlayerState->GetPlayerName();
	}*/
	/*APlayerState* PlayerState = InPawn->GetPlayerState();
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	if (PlayerState)
	{
		switch (LocalRole)
		{
		case ENetRole::ROLE_Authority:
			Role = PlayerState->GetPlayerName();
			break;
		case ENetRole::ROLE_AutonomousProxy:
			Role = PlayerState->GetPlayerName();
			break;
		case ENetRole::ROLE_SimulatedProxy:
			Role = PlayerState->GetPlayerName();
			break;
		case ENetRole::ROLE_None:
			Role = PlayerState->GetPlayerName();
			break;
		}*/

	/*}

	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	SetDisplayText(LocalRoleString);*/
}

void UGNUOverHeadWidget::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}
