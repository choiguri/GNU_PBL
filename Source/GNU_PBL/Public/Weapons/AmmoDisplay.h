// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AmmoDisplay.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UAmmoDisplay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void UpdateAmmo(int RemainAmmo, int MaxAmmo);

	void DisplayAmmo(int RemainAmmo, int MaxAmmo);

public:
	UPROPERTY(BlueprintReadonly, meta = (BindWidget))
	class UTextBlock* Ammo;
};
