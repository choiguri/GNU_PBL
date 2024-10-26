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





    //if (MeshComp && MeshComp->GetOwner())
    //{
    //    AGnuMonster* Monster = Cast<AGnuMonster>(MeshComp->GetOwner());
    //    if (Monster != nullptr)
    //    {
    //        FString NotifyName = GetNotifyName();

    //        if (NotifyName == "BreathStart")
    //        {
    //            Monster->SpawnFirebreath();  // 몬스터가 파이어브레스를 발사하는 함수
    //            GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Breath Start!"));
    //        }
    //        else if (NotifyName == "BreathEnd")
    //        {
    //            if (Monster->FirebreathActor)  // FirebreathActor가 존재하는지 확인
    //            {
    //                Monster->FirebreathActor->DestroyFirebreath();  // 브레스를 삭제
    //                Monster->FirebreathActor = nullptr;  // 참조 초기화
    //            }
    //            GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Breath End!"));
    //        }
    //        
    //    }
    //}
}
