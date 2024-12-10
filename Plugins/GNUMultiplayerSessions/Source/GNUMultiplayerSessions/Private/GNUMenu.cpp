// Fill out your copyright notice in the Description page of Project Settings.


#include "GNUMenu.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "GNUMultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "GNUFoundList.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"


void UGNUMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	//bIsFocusable = true;
	SetIsFocusable(true);

	ListBorder->SetVisibility(ESlateVisibility::Hidden);

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
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);

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
	/*if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}*/
	if (FindButton)
	{
		FindButton->OnClicked.AddDynamic(this, &ThisClass::FindButtonClicked);
	}

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseButtonClicked);
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
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red,
				FString(TEXT("Failed to create session!")));
		}
		HostButton->SetIsEnabled(true);
		
	}
}
void UGNUMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	// Session Results 중에서 설정한 MatchType과 SettingsValue 값이 같은 세션을 찾으면 JoinSession 실행
	/*for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			
			return;
		}
	}
	*/
	ListBorder->SetVisibility(ESlateVisibility::Visible);
	for (int32 i = 0; i < SessionResults.Num(); i++)
	{
		auto Result = SessionResults[i];

		FString ServerName = Result.Session.OwningUserName;
		int32 CurrentPlayer = NumPublicConnections - Result.Session.NumOpenPublicConnections;
		FString PlayerNum = FString::Printf(TEXT("%d / %d"), CurrentPlayer, NumPublicConnections);

		if (FoundListClass)
		{
			FoundList = CreateWidget<UGNUFoundList>(GetWorld(), FoundListClass);
		}
		FoundList->SetDisplayText(FoundList->ServerNameText, ServerName);
		FoundList->SetDisplayText(FoundList->PlayerText, PlayerNum);

		FoundList->OnJoinButtonClickedFunc = [this, Result]()
			{
				if (MultiplayerSessionsSubsystem)
				{
					MultiplayerSessionsSubsystem->JoinSession(Result);
				}
			};

		FoundList->JoinButton->OnClicked.AddDynamic(FoundList, &UGNUFoundList::OnJoinButtonClicked);

		if (FoundGameList)
		{
			FoundGameList->ClearChildren();
			FoundGameList->SetVisibility(ESlateVisibility::Visible);
			FoundGameList->AddChild(FoundList);
		}
		
	}
	

	/*if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}*/
}

void UGNUMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
			
		}
		
	}
	/*if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}*/
}
void UGNUMenu::OnDestroySession(bool bWasSuccessful)
{
	FoundGameList->ClearChildren();
	ListBorder->SetVisibility(ESlateVisibility::Hidden);
}
void UGNUMenu::OnStartSession(bool bWasSuccessful)
{

}

// host button 클릭 -> hostbuttonclicked 호출 -> createsession 호출 -> 세션 설정 -> 인터페이스 기능으로 세션 생성
// 1. 세션 생성 실패 시 delegate handle clear 하고 false값 broadcast -> menu에서 false값 받아서 OnCreateSession에서
// 세션 생성이 실패 되었다는 것을 수신, 아무것도 안함
// 2. 새션 생성 성공 시 delegate handle clear하고 true값 broadcast -> menu에서 true 값 받아서 OnCreateSession에서 세션 생성 성공을 수신 
// -> 세션 생성 성공했다고 스크린에 메세지
void UGNUMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

//void UGNUMenu::JoinButtonClicked()
//{
//	/*JoinButton->SetIsEnabled(false);
//	if (MultiplayerSessionsSubsystem)
//	{
//		MultiplayerSessionsSubsystem->FindSessions(10000);
//	}*/
//}

void UGNUMenu::FindButtonClicked()
{
	FoundGameList->ClearChildren();
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UGNUMenu::CloseButtonClicked()
{
	ListBorder->SetVisibility(ESlateVisibility::Hidden);
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
