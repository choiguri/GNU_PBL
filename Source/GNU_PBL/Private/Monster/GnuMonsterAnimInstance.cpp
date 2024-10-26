// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonsterAnimInstance.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/Actor.h"

void UGnuMonsterAnimInstance::SpawnFireballNotify()
{
}

void UGnuMonsterAnimInstance::AnimNotify_SpawnFireball(UAnimNotify* Notify)
{
    if (MonsterOwner)
    {
        MonsterOwner->SpawnFireball();  // 몬스터의 Fireball 생성 함수 호출
        UE_LOG(LogTemp, Warning, TEXT("Fireball Spawned from AnimNotify"));
    }
}
