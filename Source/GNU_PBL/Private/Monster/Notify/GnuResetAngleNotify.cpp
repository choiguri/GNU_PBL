// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Notify/GnuResetAngleNotify.h"
#include "Monster/GnuMonster.h"

void UGnuResetAngleNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    // Owner를 가져옴
    AGnuMonster* Monster = Cast<AGnuMonster>(MeshComp->GetOwner());
    if (Monster)
    {
        // 각도를 0으로 초기화
        /*Monster->SetDirection();*/
        UE_LOG(LogTemp, Log, TEXT("Direction reset to 0"));
    }
}
