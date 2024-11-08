// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Widget/GnuMonsterHealthBase.h"
#include "Components/ProgressBar.h"

void UGnuMonsterHealthBase::UpdateBossHP(float CurrentHP, float MaxHP)
{
    if (ProgressBar_Health)
    {
        float HPPercent = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
        ProgressBar_Health->SetPercent(HPPercent);
    }
}

void UGnuMonsterHealthBase::NativeConstruct()
{
    Super::NativeConstruct();

    // ProgressBar 값을 초기화
    if (ProgressBar_Health)
    {
        ProgressBar_Health->SetPercent(1.0f);  // 처음에 Max 상태로 설정
    }
}
