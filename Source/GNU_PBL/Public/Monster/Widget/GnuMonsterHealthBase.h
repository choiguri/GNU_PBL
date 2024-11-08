// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GnuMonsterHealthBase.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGnuMonsterHealthBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateBossHP(float CurrentHP, float MaxHP);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	class UProgressBar* ProgressBar_Health;
};
