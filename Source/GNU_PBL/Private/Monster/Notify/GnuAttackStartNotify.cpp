// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Notify/GnuAttackStartNotify.h"
#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"

void UGnuAttackStartNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        AGnuMonster* Monster = Cast<AGnuMonster>(MeshComp->GetOwner());
        UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(MeshComp->GetAnimInstance());

        if (Monster && AnimInstance)
        {
            // Attack_Start 노티파이가 불려졌을 때
            if (AnimInstance->Montage_IsPlaying(AnimInstance->ClawAttackMontage))
            {
                Monster->ActivateClawCollision(); // 클로 공격 콜리전 활성화
                GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Claw Collision Activate"));
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->TailAttackMontage))
            {
                Monster->ActivateTailCollision(); // 테일 공격 콜리전 활성화
                GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Tail Collision Activate"));
            }
            else if (AnimInstance->Montage_IsPlaying(AnimInstance->FirebreathAttackMontage))
            {
                if (Monster->FirebreathActor == nullptr)
                {
                    Monster->SpawnFirebreath();  // 몬스터가 파이어브레스를 발사하는 함수
                }
            }
        }
    }
}
