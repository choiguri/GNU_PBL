// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonsterAnimInstance.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"


// 애니메이션 상태 업데이트 로직 작성하는 곳
void UGnuMonsterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // 몬스터의 소유자를 설정합니다. 
    if (MonsterOwner == nullptr)
    {
        MonsterOwner = Cast<AGnuMonster>(TryGetPawnOwner());
    }

    // 초기값 설정
    bIsMontageEnded = true;
}


void UGnuMonsterAnimInstance::PlayMontage(UAnimMontage* MontageToPlay)
{
    if (MontageToPlay == nullptr)
    {
        return;
    }

    // 현재 몽타주가 재생 중인지 확인
    if (Montage_IsPlaying(MontageToPlay))
    {
        GEngine->AddOnScreenDebugMessage(1, 4, FColor::Red, TEXT("Playing Montage!!"));
        return;
    }

    if (bIsMontageEnded)
    {
        Montage_Play(MontageToPlay);
        bIsMontageEnded = false;
    }

    // 몽타주가 끝날 때 호출될 델리게이트를 설정
    FOnMontageEnded MontageEndedDelegate;
    MontageEndedDelegate.BindUObject(this, &UGnuMonsterAnimInstance::OnMontageEnded);

    Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
}


void UGnuMonsterAnimInstance::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 몽타주가 끝났으므로 상태 업데이트
    bIsMontageEnded = true;

    // 종료 로그 출력
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, bInterrupted ? TEXT("Montage Interrupted!") : TEXT("Montage Ended Successfully!"));
}



// 각 몽타주 실행
void UGnuMonsterAnimInstance::PlayFireballAttackMontage()
{
    PlayMontage(FireballAttackMontage);
}

void UGnuMonsterAnimInstance::PlayFlyingAttackMontage()
{
    PlayMontage(FlyingAttackMontage);
}

void UGnuMonsterAnimInstance::PlayFirebreathAttackMontage()
{
    PlayMontage(FirebreathAttackMontage);
}

void UGnuMonsterAnimInstance::PlayClawAttackMontage()
{
    PlayMontage(ClawAttackMontage);
}

void UGnuMonsterAnimInstance::PlayTailAttackMontage()
{
    PlayMontage(TailAttackMontage);
}

void UGnuMonsterAnimInstance::PlayDieMontage()
{
    PlayMontage(DragonDieMontage);
}

void UGnuMonsterAnimInstance::PlayExampleMontage()
{
    PlayMontage(ExampleAttackMontage);
}


