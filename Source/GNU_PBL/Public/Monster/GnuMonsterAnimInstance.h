// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GnuMonsterAnimInstance.generated.h"


class UCharacterMovementComponent;
class AGnuMonster;

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGnuMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    virtual void NativeInitializeAnimation() override;

    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;


    // 현재 몬스터를 참조하기 위한 변수
    UPROPERTY()
    AGnuMonster* MonsterOwner;

    UPROPERTY()
    UCharacterMovementComponent* MosnterMovementComponent;

    // 몽타주가 끝난 상태인지 확인을 위한 변수
    // 평상시 true
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage")
    bool bIsMontageEnded = true;

    // 죽음 상태인지 확인을 위한 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsDead = true;

    // 인트로 몽타주 실행 했는지 확인
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsPlayIntro = true;

    // 몽타주 실행 중인 노티파이 이벤트를 처리할 함수
    // cpp는 BlueprintNativeEvent 라고 설정되면 블루프린트에서 재정의 가능하도록 하려는 목적
    // 이 함수의 기본 구현을 C++에서 찾을 때는 _Implementation이 붙은 함수 이름을 기준으로 호출
    /*UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    void OnAttackNotifyEvent(FName NotifyName);*/


    // 애니메이션 몽타주 공격 지정
    // 
    // 근거리 공격
    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* ClawAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* TailAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* BodyAttackMontage;


    // 원거리 공격
    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* FireballAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* FlyingAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* FirebreathAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* GroundAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    UAnimMontage* GroundSpikeAttackMontage;

    // 그 외 몽타주 지정
    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* DragonDieMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* IntroShoutingMontage;

    // 임시 예시 재생 몽타주
    UPROPERTY(EditDefaultsOnly, Category = "Montage")
    UAnimMontage* ExampleAttackMontage;
    

protected:
    // 몬스터 데이터 (속도, 각도)
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData")
    float GroundSpeed;
    
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData")
    float Direction;

private:

public:
    // 애니메이션 몽타주 시작 함수
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayMontage(UAnimMontage* MontageToPlay);

    // 몽타주 애니메이션 끝났을 때 호출 될 함수
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 공격 몽타주 실행 함수
    // 근거리 공격
    UFUNCTION(BlueprintCallable)
    void PlayClawAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayTailAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayBodyAttackMontage();

    // 원거리 공격
    UFUNCTION(BlueprintCallable)
    void PlayFireballAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayFlyingAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayFirebreathAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayGroundAttackMontage();

    UFUNCTION(BlueprintCallable)
    void PlayGroundSpikeAttackMontage();

    // 죽었을 때 몽타주 처리
    UFUNCTION(BlueprintCallable)
    void PlayDieMontage();

    // 첫 입장 시 나오는 몽타주
    UFUNCTION(BlueprintCallable)
    void PlayIntroMontage();

    // 애니메이션 예시 실행기
    UFUNCTION(BlueprintCallable)
    void PlayExampleMontage();
};
