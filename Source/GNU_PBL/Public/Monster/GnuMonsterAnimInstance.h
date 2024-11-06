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
    virtual void NativeInitializeAnimation() override;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    void OnAttackNotifyEvent(FName NotifyName);


    // 현재 몬스터를 참조하기 위한 변수
    UPROPERTY(BlueprintReadOnly, Category = "Monster")
    class AGnuMonster* MonsterOwner;

    // 애니메이션 몽타주 시작
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayAttackMontage(UAnimMontage* MontageToPlay);

    // 현재 재생 중인 애니메이션 몽타주 가져오기
    UFUNCTION(BlueprintCallable, Category = "Animation")
    UAnimMontage* GetCurrentMontage() const;


    // 애니메이션 몽타주 공격 지정
    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* FireballAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* FlyingAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* FirebreathAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* ClawAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* TailAttackMontage;
    

protected:
    // 몽타주 애니메이션을 재생하기 위한 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;
    

private:
    // 공격 몽타주 실행 중에 특정 로직을 실행하는 함수
    void HandleAttackNotify(FName NotifyName);

public:

    // 공격 몽타주 실행 함수
    UFUNCTION(BlueprintCallable)
    void PlayFireballAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayFlyingAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayFirebreathAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayClawAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayTailAttackMontage();
};
