// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GnuMyCharacterAnimInstance.generated.h"

class AGnuMyCharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EMovementInput : uint8
{
	Forward        UMETA(DisplayName = "Forward"),
	Right     UMETA(DisplayName = "Right"),
	Backward     UMETA(DisplayName = "Backward"),
	Left     UMETA(DisplayName = "Left")
};

UCLASS()
class GNU_PBL_API UGnuMyCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:	
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	AGnuMyCharacter* MyCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* MyCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed; // 캐릭터의 바닥 속도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector Velocity; // 캐릭터의 바닥 속도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool ShouldMove; // 캐릭터의 움직임 여부
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool IsFalling; // 떠있는지 여부

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Direction; // 캐릭터가 바라보는 방향
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EMovementInput MovementInput; //열거형 변수 선언

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float F_Orientation_Angle; // 앞 방향의 각도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float R_Orientation_Angle; // 오른쪽 방향의 각도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float B_Orientation_Angle; // 뒤 방향의 각도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float L_Orientation_Angle; // 왼쪽 방향의 각도

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Yaw; // 왼쪽 방향의 각도
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float YawLastTick; // 왼쪽 방향의 각도
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float YawChangeOver; // 왼쪽 방향의 각도
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float YawOffset; // 왼쪽 방향의 각도
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurveValue; // 왼쪽 방향의 각도
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastCurveValue; // 왼쪽 방향의 각도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isSprinting; // 달리는지 여부

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isCrouching; // 앉았는지 여부

	bool DoOnce;

public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeInitializeAnimation() override;

	void SwitchSprint();
	void SwitchCrouch();
	bool GetIsSprinting();
	bool GetIsCroucing();
};
