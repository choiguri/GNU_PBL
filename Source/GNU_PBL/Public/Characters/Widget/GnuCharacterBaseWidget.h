// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GnuCharacterBaseWidget.generated.h"

UCLASS()
class GNU_PBL_API UGnuCharacterBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthBar(float CurrentHP, float MaxHP);
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateStaminaBar(float CurrentStamina, float MaxStamina);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	class UProgressBar* ProgressBar_Health;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	class UProgressBar* ProgressBar_Stamina;
	
};

/*
< meta = (BindWidget) >
- BindWidget은 UI 위젯을 C++ 클래스와 연결하는 데 사용 ->  UI와 C++ 코드 간의 상호작용을 가능
- BindWidget으로 바인딩된 변수는 BlueprintReadOnly로 선언되어야 하므로, 값을 변경할 수 없고 읽기 전용으로 사용
*/