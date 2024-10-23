// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GnuMyCharacterAnimInstance.generated.h"

class AGnuMyCharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType) 
enum class EMovementInput : uint8 // 캐릭터의 이동 방향 상태
{
	Forward        UMETA(DisplayName = "Forward"),
	Right     UMETA(DisplayName = "Right"),
	Backward     UMETA(DisplayName = "Backward"),
	Left     UMETA(DisplayName = "Left")
};

UENUM(BlueprintType)
enum class EAnimationState : uint8 // 캐릭터의 현재 애니메이션 상태
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
	FVector CurVelocity; // 캐릭터의 현재 속도 벡터를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector CurAcceleration; // 캐릭터의 현재 가속도 벡터를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Direction; // 캐릭터가 바라보는 방향을 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float F_Orientation_Angle; // 캐릭터가 앞쪽으로 바라보는 방향의 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float R_Orientation_Angle; // 캐릭터가 오른쪽으로 바라보는 방향의 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float B_Orientation_Angle; // 캐릭터가 뒤쪽으로 바라보는 방향의 각도를 저장하는 변수

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float L_Orientation_Angle; // 캐릭터가 왼쪽으로 바라보는 방향의 각도를 저장하는 변수

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

public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeInitializeAnimation() override;

	void SetTurnRate(float CurYaw);
	void SetWeapon();
	
	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerSetWeapon(); // 서버에서 구르기 몽타주를 실행하는 함수 (컨트롤러에서 호출)
	//UFUNCTION(NetMulticast, Reliable)
	//void MultiCastSetWeapon(); // 구르기 몽타주 멀티캐스트 (서버가 호출되면 자동으로 호출)

	UPROPERTY(ReplicatedUsing = OnRep_SetAnimState, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EAnimationState EAnimState; // 달리는지 여부
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAnimState(); // 서버 앉기 시작 (컨트롤러에서 호출)
	UFUNCTION(Client, Reliable)
	void ClientSetAnimState(); // 클라리언트 앉기 시작 (서버가 호출되면 자동으로 호출)
	UFUNCTION()
	void OnRep_SetAnimState(); // isCrouch 변수가 변경되면 호출되는 함수 (이 함수에서 실행되는 것들이 클라이언트 쪽에서 실행되는 것들)

private:
	// 네트워크 복제를 위한 함수 선언 : UFUNCTION(Server) 기능 사용하려면 이 함수가 있어야 함
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateMovementState(); // 이동 상태 업데이트
	void UpdateDirectionAndMovementInput(); // 방향과 이동 입력을 업데이트

};
