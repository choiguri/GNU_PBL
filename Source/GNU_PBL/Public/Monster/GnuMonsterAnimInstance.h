// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GnuMonsterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGnuMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    // 파이어볼 스폰 함수를 AnimInstance에 추가
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void SpawnFireballNotify();  // Notify에서 호출될 함수 선언

    UFUNCTION()
    void AnimNotify_SpawnFireball(UAnimNotify* Notify);  // AnimNotify 이벤트 처리 함수


private:
    // 몬스터를 참조하기 위한 변수
    class AGnuMonster* MonsterOwner;
};
