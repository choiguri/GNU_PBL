// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Notify/GnuAttackEndNotify.h"
#include "Monster/AttackActor/GnuFirebreathActor.h"
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
                Monster->DeactivateClawCollision(); // 손톱 공격 종료 로직
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->TailAttackMontage))
            {
                Monster->DeactivateTailCollision(); // 꼬리 공격 종료 로직
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->BodyAttackMontage))
            {
                Monster->DeactivateBodyCollision(); // 몸통 공격 종료 로직
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->FlyingDodgeMontage))
            {
                Monster->EndCraterAttack(); // 분화구 스폰 종료
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->FirebreathAttackMontage))
            {
                Monster->FirebreathActor->DestroyFirebreath(); // 소환된 브레스 액터 삭제
                Monster->FirebreathActor = nullptr; // 참조 초기화는 필요하지 않을 수 있음
            }
        }
    }
}
