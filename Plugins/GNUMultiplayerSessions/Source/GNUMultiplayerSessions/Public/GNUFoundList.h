// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GNUFoundList.generated.h"

/**
 * 
 */
UCLASS()
class GNUMULTIPLAYERSESSIONS_API UGNUFoundList : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerText;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	TFunction<void()> OnJoinButtonClickedFunc;

	UFUNCTION()
	void OnJoinButtonClicked();

	void SetDisplayText(UTextBlock* TextBlock, FString TextToDisplay);

};
