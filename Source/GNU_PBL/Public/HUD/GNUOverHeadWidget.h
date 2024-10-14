// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GNUOverHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGNUOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* InPawn);

	
protected:
	virtual void NativeDestruct() override;
};
