// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

// PlayerController 합치면서 추가
#include "GameFramework/PlayerState.h"
#include "InputActionValue.h"

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


	// 추가사항
	void SetHUDHealth(float Health, float MaxHealth);

	void SetHUDStamina(float Stamina, float MaxStamina);

	void SetHUDCombatTime(float CombatTime);

	void SetHUDWeaponAmmo(int32 Ammo, int32 MaxAmmo);

	virtual void Tick(float DeltaTime) override;
	virtual float GetServertime();
	virtual void ReceivedPlayer() override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//
	// 추가사항
	//
	void SetHUDTime();
	void ShowReturnToMainMenu();

	// 서버와 클라이언트 사이의 시간 동기화
	// 현재 서버 시간 요청
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// 현재 서버 시간과 서버에서 클라이언트로의 응답시간 
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // 서버와 클라이언트 사이의 차이

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.0f;
	void CheckTimeSync(float DeltaTime);


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



	// 추가사항 GnuWeapon
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> QuitAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> Crouch;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> WeaponReloadAction;

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
	// 기존
	void Aiming();
	void StopAiming();
	//void Fire();
	//void StopFire();
	//void Reload();
	//void Interact(); // 상호작용 함수
	// -----------------------------------------------

	// GnuWeapon
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
//
// 추가사항
//
private:
	UPROPERTY()
	class AGNUHUD* GNUHUD;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UGNUReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

	float TotalTime = 120.f; // 나중에 total 게임 시간 설정 해야 함
	uint32 CountdownInt = 0;
};

// TObjectPtr :  ����Ʈ ������ Ÿ������, �ַ� UObject ��� ��ü�� �����ϰ� �����ϴ� �� ���
// Unreal Engine�� ������ �÷��� �ý��۰� ���յǾ�, ��ü�� �� �̻� �ʿ����� ���� �� �ڵ����� �޸𸮸� ������ �� �ֵ��� �����ϴ�. 
// -> ������ �÷��� : ��ü�� ���� ī��Ʈ�� �����Ͽ�, �� �̻� �������� �ʴ� ��ü�� �ڵ����� û��
// Ư�� UObject Ÿ�Ը� ����ų �� �ֵ��� �����մϴ�. �̴� �ٸ� Ÿ���� ��ü�� �߸� �����ϴ� ������ ����, ������ Ÿ�ӿ��� ������ �̸� ����
// -> ���� ���, UInputAction Ÿ���� ��ü�� �ƴ� �ٸ� ��ü�� �Ҵ��Ϸ��� �ϸ� ������ ������ �߻�
