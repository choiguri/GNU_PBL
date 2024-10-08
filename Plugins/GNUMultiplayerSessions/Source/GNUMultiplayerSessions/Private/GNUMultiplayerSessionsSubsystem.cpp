// Fill out your copyright notice in the Description page of Project Settings.


#include "GNUMultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

UGNUMultiplayerSessionsSubsystem::UGNUMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UGNUMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{

}

void UGNUMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{

}

void UGNUMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{

}

void UGNUMultiplayerSessionsSubsystem::DestroySession()
{

}

void UGNUMultiplayerSessionsSubsystem::StartSession()
{

}

void UGNUMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{

}

void UGNUMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{

}

void UGNUMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{

}

void UGNUMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

}

void UGNUMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{

}
