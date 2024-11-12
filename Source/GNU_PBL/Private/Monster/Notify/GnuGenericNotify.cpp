// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Notify/GnuGenericNotify.h"
#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"


void UGnuGenericNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        AGnuMonster* Monster = Cast<AGnuMonster>(MeshComp->GetOwner());
        if (Monster != nullptr)
        {
            UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(MeshComp->GetAnimInstance());
            if (AnimInstance)
            {
                if (AnimInstance->Montage_IsPlaying(AnimInstance->FireballAttackMontage))
                {
                    Monster->SpawnFireball();  // 몬스터가 파이어볼을 발사하는 함수
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->FlyingAttackMontage))
                {
                    Monster->SpawnFiretornado();
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->GroundAttackMontage))
                {
                    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, TEXT("Ground Attack Actor Spawn"));
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->DragonDieMontage))
                {
                    AnimInstance->Montage_Pause(AnimInstance->DragonDieMontage);
                    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Die Montage Pause"));
                }
            }
        }
    }
}
