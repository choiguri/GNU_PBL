// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/GNUReturnToMainMenu.h"
#include "GnuResult.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGnuResult : public UGNUReturnToMainMenu
{
	GENERATED_BODY()
	
public:
	void SetVictoryText();
	void SetDefeatText();

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResultText;
};
