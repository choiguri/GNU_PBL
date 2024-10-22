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

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	Unarmed        UMETA(DisplayName = "Unarmed"),
	Pistol     UMETA(DisplayName = "Pistol"),
	Rifle     UMETA(DisplayName = "Rifle"),
};

UCLASS()
class GNU_PBL_API UGnuMyCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:	
	// 네트워크 복제를 위한 함수 선언 : UFUNCTION(Server) 기능 사용하려면 이 함수가 있어야 함
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	AGnuMyCharacter* MyCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* MyCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed; // 캐릭터의 바닥 속도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector CurVelocity; // 

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector CurAcceleration; //

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

	// ------------ Turn In Place에 사용되는 변수들 (c++로 구현 못해서 애니메이션 블프로 해둠) ---------------
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Yaw;
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float YawLastTick;
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float YawChangeOver;
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float YawOffset;
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurveValue;
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LastCurveValue;
	//--------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurPitch;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float TurnRate;
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurDirectionAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isSprinting; // 달리는지 여부

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isCrouching; // 앉았는지 여부;


	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EAnimationState EAnimState;

public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeInitializeAnimation() override;

	void SetTurnRate(float CurYaw);
	void SetWeapon();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetWeapon(); // 서버에서 구르기 몽타주를 실행하는 함수 (컨트롤러에서 호출)
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastSetWeapon(); // 구르기 몽타주 멀티캐스트 (서버가 호출되면 자동으로 호출)

};
