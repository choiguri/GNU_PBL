// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "GNUMenu.generated.h"

/**
 * 
 */

UCLASS()
class GNUMULTIPLAYERSESSIONS_API UGNUMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/GNU/Maps/Lobby")));

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> FoundListClass;

	class UGNUFoundList* FoundList;



protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	// if meta is BindWidget, WBP and C++ must have same name
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	//UPROPERTY(meta = (BindWidget))
	//UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FindButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* FoundGameList;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ListBorder;

	UFUNCTION()
	void HostButtonClicked();

	//UFUNCTION()
	//void JoinButtonClicked();

	UFUNCTION()
	void FindButtonClicked();

	UFUNCTION()
	void CloseButtonClicked();

	void MenuTearDown();
	
	// The subsystem designed to hanle all online session functionality
	class UGNUMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FString PathToLobby{TEXT("")};
};
