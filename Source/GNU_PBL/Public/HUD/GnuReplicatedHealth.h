// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GnuReplicatedHealth.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGnuReplicatedHealth : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ReplicatedHealth;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReplicatedHealthText;

	UFUNCTION(BlueprintCallable)
	void SetHealthText(FString TextToDisplay);

protected:
	virtual void NativeDestruct() override;

	
};
