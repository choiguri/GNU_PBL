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
	
};
