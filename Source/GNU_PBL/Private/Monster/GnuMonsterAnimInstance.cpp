// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonsterAnimInstance.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/Actor.h"
#include "Monster/GnuMonster.h"
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
}


void UGnuMonsterAnimInstance::OnAttackNotifyEvent_Implementation(FName NotifyName)
{
    HandleAttackNotify(NotifyName);
}


void UGnuMonsterAnimInstance::HandleAttackNotify(FName NotifyName)
{
    if (NotifyName == "Attack_Start")
    {
        UE_LOG(LogTemp, Log, TEXT("Attack Start Notify triggered."));
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Attack Start Notify trggered"));
        if (MonsterOwner)
        {
            MonsterOwner->ActivateClawCollision();
        }
    }
    else if (NotifyName == "Attack_End")
    {
        UE_LOG(LogTemp, Log, TEXT("Attack End Notify triggered."));
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Attack End Notify trggered"));
        if (MonsterOwner)
        {
            MonsterOwner->DeactivateClawCollision();
        }
    }
}


void UGnuMonsterAnimInstance::PlayAttackMontage(UAnimMontage* MontageToPlay)
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

    // 새 몽타주 재생
    Montage_Play(MontageToPlay);
}

UAnimMontage* UGnuMonsterAnimInstance::GetCurrentMontage() const
{
    return nullptr;
}


// 각 몽타주 실행
void UGnuMonsterAnimInstance::PlayFireballAttackMontage()
{
    PlayAttackMontage(FireballAttackMontage);
}

void UGnuMonsterAnimInstance::PlayFlyingAttackMontage()
{
    PlayAttackMontage(FlyingAttackMontage);
}

void UGnuMonsterAnimInstance::PlayFirebreathAttackMontage()
{
    PlayAttackMontage(FirebreathAttackMontage);
}

void UGnuMonsterAnimInstance::PlayClawAttackMontage()
{
    PlayAttackMontage(ClawAttackMontage);
}

void UGnuMonsterAnimInstance::PlayTailAttackMontage()
{
    PlayAttackMontage(TailAttackMontage);
}

void UGnuMonsterAnimInstance::PlayExampleMontage()
{
    PlayAttackMontage(ExampleAttackMontage);
}


