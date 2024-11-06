// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Notify/GnuFirebreathNotify.h"
#include "Monster/AttackActor/GnuFirebreathActor.h"
#include "Monster/GnuMonster.h"

void UGnuFirebreathNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        AGnuMonster* Monster = Cast<AGnuMonster>(MeshComp->GetOwner());
        if (Monster != nullptr)
        {
            if (Monster->FirebreathActor == nullptr)
            {
                Monster->SpawnFirebreath();  // 몬스터가 파이어브레스를 발사하는 함수
            }
            else
            {
                Monster->FirebreathActor->DestroyFirebreath();
                Monster->FirebreathActor = nullptr;  // 참조 초기화
            }
        }
    }
}
