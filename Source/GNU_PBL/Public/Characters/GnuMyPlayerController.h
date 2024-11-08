// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GnuMyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class GNU_PBL_API AGnuMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGnuMyPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputMappingContext> MyCharacterContext;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> MoveAction; // �̵�

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction; // ȸ��

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> JumpAction; // ����

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SprintAction; // �޸���

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> CrouchAction; // �ɱ�

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> DodgeAction; // ������

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ToggleCameraAction; // ī�޶� ��Ī ��ȯ

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> WeaponChangeAction; // ���� ��ȯ (���� q�� ���ʴ�� �ٲ�� �����ص�)

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ZoomInAction; // ī�޶� ��

	// Weapon input
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> AimingAction;
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ShotAction;
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ReloadAction;
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> InteractAction; // 상호작용 인풋
	
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ArrowSkillAction; // ȭ�� ��ų


	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> HealSkillAction; // �� ��ų
	FVector2D CurrentMoveDirection; // ���� �̵� ���� Dodge�� �� ���


	void Move(const struct FInputActionValue& InputActionValue); // �̵�
	void Rotation(const struct FInputActionValue& InputActionValue); // ȸ��
	void Dodge(const struct FInputActionValue& InputActionValue); // ������
	void Sprint(const struct FInputActionValue& InputActionValue); // �޸���
	void SprintStop(const struct FInputActionValue& InputActionValue); // �޸��� ��
	void ToggleCrouch(const struct FInputActionValue& InputActionValue); // �ɱ�
	void ToggleCamera(const struct FInputActionValue& InputActionValue); // ī�޶� ��Ī ��ȯ
	void ToggleZoomIn(const struct FInputActionValue& InputActionValue); // ī�޶� ����
	void WeaponChange(const struct FInputActionValue& InputActionValue); // ���� ü���� (PistolChange, RifleChange�� �͵� ���� �Ҳ��� ���� �ؾ���)
	void Jump(const struct FInputActionValue& InputActionValue); // ����
	void StopJumping(const struct FInputActionValue& InputActionValue); // ���� ��
	
	void ArrowSkill(const struct FInputActionValue& InputActionValue); // ���ο� ��ų
	void HealSkill(const struct FInputActionValue& InputActionValue); // �� ��ų

	// ------------------ Weapon funtion -------------
	void Aiming();
	void StopAiming();
	void Fire();
	void StopFire();
	void Reload();
	void Interact(); // 상호작용 함수
	// -----------------------------------------------
};

// TObjectPtr :  ����Ʈ ������ Ÿ������, �ַ� UObject ��� ��ü�� �����ϰ� �����ϴ� �� ���
// Unreal Engine�� ������ �÷��� �ý��۰� ���յǾ�, ��ü�� �� �̻� �ʿ����� ���� �� �ڵ����� �޸𸮸� ������ �� �ֵ��� �����ϴ�. 
// -> ������ �÷��� : ��ü�� ���� ī��Ʈ�� �����Ͽ�, �� �̻� �������� �ʴ� ��ü�� �ڵ����� û��
// Ư�� UObject Ÿ�Ը� ����ų �� �ֵ��� �����մϴ�. �̴� �ٸ� Ÿ���� ��ü�� �߸� �����ϴ� ������ ����, ������ Ÿ�ӿ��� ������ �̸� ����
// -> ���� ���, UInputAction Ÿ���� ��ü�� �ƴ� �ٸ� ��ü�� �Ҵ��Ϸ��� �ϸ� ������ ������ �߻�
