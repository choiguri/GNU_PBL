// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void MenuSetup();

protected:

	virtual bool Initialize() override;

private:
	// if meta is BindWidget, WBP and C++ must have same name
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();
	
	// The subsystem designed to hanle all online session functionality
	class UGNUMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
};
