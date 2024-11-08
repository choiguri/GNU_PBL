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


    // 몽타주 실행 중인 노티파이 이벤트를 처리할 함수
    // cpp는 BlueprintNativeEvent 라고 설정되면 블루프린트에서 재정의 가능하도록 하려는 목적
    // 이 함수의 기본 구현을 C++에서 찾을 때는 _Implementation이 붙은 함수 이름을 기준으로 호출
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

    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* ExampleAttackMontage;
    

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

    UFUNCTION(BlueprintCallable)
    void PlayExampleMontage();
};
