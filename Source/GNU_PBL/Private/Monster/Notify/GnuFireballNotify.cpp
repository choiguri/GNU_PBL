// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/Notify/GnuFireballNotify.h"
#include "Monster/GnuMonster.h"

void UGnuFireballNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        AGnuMonster* Monster = Cast<AGnuMonster>(MeshComp->GetOwner());
        if (Monster != nullptr)
        {
            Monster->SpawnFireball();  // 몬스터가 파이어볼을 발사하는 함수
        }
    }
}
