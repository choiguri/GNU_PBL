// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GnuMyAnimInstance.generated.h"

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
class GNU_PBL_API UGnuMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	AGnuMyCharacter* MyCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* MyCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EMovementInput MovementInput; // 캐릭터의 이동 방향을 저장하는 열거형 변수

	//UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	//EAnimationState EAnimState; // 캐릭터의 현재 애니메이션 상태를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed; // 캐릭터의 바닥 속도

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector CurVelocity;  // 캐릭터의 현재 속도 벡터를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector CurAcceleration;// 캐릭터의 현재 가속도 벡터를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Direction;// 캐릭터가 바라보는 방향을 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float F_Orientation_Angle;// 캐릭터가 앞쪽으로 바라보는 방향의 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float R_Orientation_Angle; // 캐릭터가 오른쪽으로 바라보는 방향의 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float B_Orientation_Angle; // 캐릭터가 뒤쪽으로 바라보는 방향의 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float L_Orientation_Angle;  // 캐릭터가 왼쪽으로 바라보는 방향의 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurPitch; // 캐릭터의 현재 피치 값을 저장하는 변수 (고개를 돌리는 각도)

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float TurnRate; // 캐릭터의 현재 회전 속도(턴 속도)를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurDirectionAngle; // 캐릭터의 현재 이동 방향 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool ShouldMove; // 캐릭터가 움직이는지 여부를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool IsFalling; // 캐릭터가 공중에 떠 있는지 여부를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	

public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeInitializeAnimation() override;

	void SetTurnRate(float CurYaw);
	void SetWeapon();

	UPROPERTY(ReplicatedUsing = OnRep_SetAnimState, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EAnimationState EAnimState;
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAnimState();
	UFUNCTION(Client, Reliable)
	void ClientSetAnimState();
	UFUNCTION()
	void OnRep_SetAnimState();

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateMovementState(); // 이동 상태 업데이트
	void UpdateDirectionAndMovementInput(); // 방향과 이동 입력을 업데이트

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;

	class AGnuWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FRotator RightHandRotation;
};
