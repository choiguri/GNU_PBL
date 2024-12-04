// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Notify/GnuGenericNotify.h"
#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"
#include "Characters/GnuMyCharacter.h"
#include "Kismet/GameplayStatics.h"


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
                    Monster->SpawnFireball(); 
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->FireballAttackContinuousMontage))
                {
                    Monster->SpawnFireball();
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->FlyingAttackMontage))
                {
                    Monster->SpawnFiretornado();
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->GroundAttackMontage))
                {
                    Monster->SpawnGroundAttack();
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->GroundSpikeAttackMontage))
                {
                    Monster->SpawnGroundSpikeAttack();
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->DragonDieMontage))
                {
                    AnimInstance->Montage_Pause(AnimInstance->DragonDieMontage);
                    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Die Montage Pause"));
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->IntroShoutingMontage))
                {
                    // 플레이어 캐릭터를 찾기 위해 GetWorld() 사용
                    AGnuMyCharacter* Character = Cast<AGnuMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
                    if (Character)
                    {
                        Character->PlayCameraShake(); // Notify 시 카메라 쉐이크 실행
                        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Play Camera Shake"));
                    }
                    else
                    {
                    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Character not found"));
                    }
                }
                else if (AnimInstance->Montage_IsPlaying(AnimInstance->BodyAttackMontage))
                {
                    Monster->BodyAttack();
                }
            }
        }
    }
}