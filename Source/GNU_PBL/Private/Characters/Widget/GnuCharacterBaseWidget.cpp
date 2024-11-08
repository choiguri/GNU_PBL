// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Widget/GnuCharacterBaseWidget.h"
#include "Components/ProgressBar.h"
#include "Net/UnrealNetwork.h" // Replication에 필요

void UGnuCharacterBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // ProgressBar 값을 초기화
    if (ProgressBar_Health)
    {
        ProgressBar_Health->SetPercent(1.0f);  // 처음에 Max 상태로 설정
    }
    if (ProgressBar_Stamina)
    {
        ProgressBar_Stamina->SetPercent(1.0f);  // 처음에 Max 상태로 설정
    }
}

// 체력 바 업데이트 함수
void UGnuCharacterBaseWidget::UpdateHealthBar(float CurrentHP, float MaxHP)
{
    if (ProgressBar_Health)
    {
        // 현재 체력 비율을 계산하여 체력 바 업데이트
        float HPPercent = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
        ProgressBar_Health->SetPercent(HPPercent);
    }
}

// 스태미너 바 업데이트 함수
void UGnuCharacterBaseWidget::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{
    if (ProgressBar_Stamina && MaxStamina > 0.0f)
    {
        // 현재 스태미너 비율을 계산하여 스태미너 바 업데이트
        float StaminaPercent = FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f);
        ProgressBar_Stamina->SetPercent(StaminaPercent);
    }
}
