// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GnuMyAnimInstance.generated.h"

class AGnuMyCharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EMovementInput : uint8 // ĳ������ �̵� ���� ����
{
	Forward        UMETA(DisplayName = "Forward"),
	Right     UMETA(DisplayName = "Right"),
	Backward     UMETA(DisplayName = "Backward"),
	Left     UMETA(DisplayName = "Left")
};

UENUM(BlueprintType)
enum class EAnimationState : uint8 // ĳ������ ���� �ִϸ��̼� ����
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
	EMovementInput MovementInput; // ĳ������ �̵� ������ �����ϴ� ������ ����

	//UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	//EAnimationState EAnimState; // ĳ������ ���� �ִϸ��̼� ���¸� �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed; // ĳ������ �ٴ� �ӵ�

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector CurVelocity; // ĳ������ ���� �ӵ� ���͸� �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FVector CurAcceleration; // ĳ������ ���� ���ӵ� ���͸� �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Direction; // ĳ���Ͱ� �ٶ󺸴� ������ �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float F_Orientation_Angle; // ĳ���Ͱ� �������� �ٶ󺸴� ������ ������ �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float R_Orientation_Angle; // ĳ���Ͱ� ���������� �ٶ󺸴� ������ ������ �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float B_Orientation_Angle; // ĳ���Ͱ� �������� �ٶ󺸴� ������ ������ �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float L_Orientation_Angle; // ĳ���Ͱ� �������� �ٶ󺸴� ������ ������ �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurPitch; // ĳ������ ���� ��ġ ���� �����ϴ� ���� (����� ������ ����)

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float TurnRate; // ĳ������ ���� ȸ�� �ӵ�(�� �ӵ�)�� �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurDirectionAngle; // ĳ������ ���� �̵� ���� ������ �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool ShouldMove; // ĳ���Ͱ� �����̴��� ���θ� �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool IsFalling; // ĳ���Ͱ� ���߿� �� �ִ��� ���θ� �����ϴ� ����

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	

public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeInitializeAnimation() override;

	void SetTurnRate(float CurYaw);
	void SetWeapon();

	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerSetWeapon(); // �������� ������ ��Ÿ�ָ� �����ϴ� �Լ� (��Ʈ�ѷ����� ȣ��)
	//UFUNCTION(NetMulticast, Reliable)
	//void MultiCastSetWeapon(); // ������ ��Ÿ�� ��Ƽĳ��Ʈ (������ ȣ��Ǹ� �ڵ����� ȣ��)

	UPROPERTY(ReplicatedUsing = OnRep_SetAnimState, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EAnimationState EAnimState; // �޸����� ����
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAnimState(); // ���� �ɱ� ���� (��Ʈ�ѷ����� ȣ��)
	UFUNCTION(Client, Reliable)
	void ClientSetAnimState(); // Ŭ�󸮾�Ʈ �ɱ� ���� (������ ȣ��Ǹ� �ڵ����� ȣ��)
	UFUNCTION()
	void OnRep_SetAnimState(); // isCrouch ������ ����Ǹ� ȣ��Ǵ� �Լ� (�� �Լ����� ����Ǵ� �͵��� Ŭ���̾�Ʈ �ʿ��� ����Ǵ� �͵�)

private:
	// ��Ʈ��ũ ������ ���� �Լ� ���� : UFUNCTION(Server) ��� ����Ϸ��� �� �Լ��� �־�� ��
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateMovementState(); // �̵� ���� ������Ʈ
	void UpdateDirectionAndMovementInput(); // ����� �̵� �Է��� ������Ʈ

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;
};
