// Fill out your copyright notice in the Description page of Project Settings.

//GnuCharacterAnimInstance.h


#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GnuCharacterAnimInstance.generated.h"

class AGnuCharacter;
class UCharacterMovementComponent;

UCLASS()
class GNU_PBL_API UGnuCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
	AGnuCharacter* MyCharacter;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* MyCharacterMovement;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float MoveLength;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isJump;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isStraightMove = true;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isBackMove = false;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isLeftMove = false;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isRightMove = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isRoll = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* RollMontage;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void SetIsStraightMove(bool Value);
	void SetIsBackMove(bool Value);
	void SetIsLeftMove(bool Value);
	void SetIsRightMove(bool Value);
	bool isWalking();
	bool isRolling();

	// 구르기 애니메이션 재생 함수 추가
	void PlayRollMontage();

	// 구르기 몽타주가 현재 재생 중인지 확인하는 함수 추가
	bool IsPlayingAnyMontage() const;
};
