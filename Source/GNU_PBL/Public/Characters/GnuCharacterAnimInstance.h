// Fill out your copyright notice in the Description page of Project Settings.

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
	float AimPitch;


public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void SetIsStraightMove(bool Value);
	void SetIsBackMove(bool Value);
	void SetIsLeftMove(bool Value);
	void SetIsRightMove(bool Value);
	void SetAimPitch(float Value);
	bool isWalking();
};
