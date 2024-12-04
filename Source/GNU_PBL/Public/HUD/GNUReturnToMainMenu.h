// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GNUReturnToMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGNUReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void MenuSetup();
	void MenuTearDown();

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ReturnButton;

	UFUNCTION()
	void ReturnButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

	UFUNCTION()
	void RestartButtonClicked();

	UPROPERTY()
	class UGNUMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
	class APlayerController* PlayerController;


};
