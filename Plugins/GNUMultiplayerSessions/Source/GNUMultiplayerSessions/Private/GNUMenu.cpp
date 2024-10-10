// Fill out your copyright notice in the Description page of Project Settings.


#include "GNUMenu.h"
#include "Components/Button.h"
#include "GNUMultiplayerSessionsSubsystem.h"

void UGNUMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	//bIsFocusable = true;
	SetIsFocusable(true);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData); // In InputMode, can't control Player
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UGNUMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
	}
}

bool UGNUMenu::Initialize()
{
	if (!Super::Initialize()) // Super의 Initialize가 false 이면 return false
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

// NativeDestruct는 RemoveFromParent 시 실행됨 
void UGNUMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

void UGNUMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
				FString(TEXT("Session created successfully!")));
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/GNU/Maps/Lobby?listen");
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
				FString(TEXT("Failed to create session!")));
		}
	}
}
// host button 클릭 -> hostbuttonclicked 호출 -> createsession 호출 -> 세션 설정 -> 인터페이스 기능으로 세션 생성
// 1. 세션 생성 실패 시 delegate handle clear 하고 false값 broadcast -> menu에서 false값 받아서 OnCreateSession에서
// 세션 생성이 실패 되었다는 것을 수신, 아무것도 안함
// 2. 새션 생성 성공 시 delegate handle clear하고 true값 broadcast -> menu에서 true 값 받아서 OnCreateSession에서 세션 생성 성공을 수신 
// -> 세션 생성 성공했다고 스크린에 메세지
void UGNUMenu::HostButtonClicked()
{
	
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UGNUMenu::JoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow,
			FString::Printf(TEXT("Join Button Clicked")));
	}
}

void UGNUMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
