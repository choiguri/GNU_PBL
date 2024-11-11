// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Notify/GnuAttackEndNotify.h"
#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"

void UGnuAttackEndNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        AGnuMonster* Monster = Cast<AGnuMonster>(MeshComp->GetOwner());
        UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(MeshComp->GetAnimInstance());

        if (Monster && AnimInstance)
        {
            // Attack_End 노티파이가 불려졌을 때
            if (AnimInstance->Montage_IsPlaying(AnimInstance->ClawAttackMontage))
            {
                Monster->DeactivateClawCollision(); // 클로 공격 종료 로직
                GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Claw Collision Deactivate"));
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->TailAttackMontage))
            {
                Monster->DeactivateTailCollision(); // 테일 공격 종료 로직
                GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Tail Collision Deactivate"));
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->DragonDieMontage))
            {
                AnimInstance->Montage_Pause(AnimInstance->DragonDieMontage);
                GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Die Montage Pause"));
            }
        }
    }
}
