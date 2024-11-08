// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GnuBaseCharacter.h"
#include "Components/TimelineComponent.h" // FTimeline
#include "GnuMyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class AGun;
class UFTimeline;
class UCurveFloat;
class AGnuProjectileActor;
class AGnuHealActor;
class UGnuCharacterBaseWidget;

UCLASS()
class GNU_PBL_API AGnuMyCharacter : public AGnuBaseCharacter
{
	GENERATED_BODY()

private:
	// ��Ʈ��ũ ������ ���� �Լ� ���� : UFUNCTION(Server) ��� ����Ϸ��� �� �Լ��� �־�� ��
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	AGnuMyCharacter();

	// --------------------------------------------- Sprint ��� -----------------------------------
	// ReplicatedUsing = OnRep_IsSprinting : �̷��� �����ϸ� ���� ������ ������ �ڵ����� OnRep_IsSprinting�Լ��� ȣ��ȴ�.
	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isSprint;  // �޸����� ���� (GnuMyCharacterAnimInstance���� ��� ��)
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintStart(); // ���� �޸��� ���� (��Ʈ�ѷ����� ȣ��)
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintEnd(); // ���� �޸��� �� (��Ʈ�ѷ����� ȣ��)
	UFUNCTION(Client, Reliable)
	void ClientSprintStart(); // Ŭ���̾�Ʈ �޸��� ���� (������ ȣ��Ǹ� �ڵ����� ȣ��)
	UFUNCTION(Client, Reliable)
	void ClientSprintEnd(); // Ŭ���̾�Ʈ �޸��� �� (������ ȣ��Ǹ� �ڵ����� ȣ��)
	UFUNCTION()
	void OnRep_IsSprinting(); // isSprint ������ ����Ǹ� ȣ��Ǵ� �Լ� (�� �Լ����� ����Ǵ� �͵��� Ŭ���̾�Ʈ �ʿ��� ����Ǵ� �͵�)
	void UpdateSprintState(bool bIsSprinting);
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------- Crouch ��� ------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_IsCrouching, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isCrouch;  // �޸����� ����
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrouchStart(); // ���� �ɱ� ���� (��Ʈ�ѷ����� ȣ��)
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrouchEnd(); // ���� �ɱ� �� (��Ʈ�ѷ����� ȣ��)
	UFUNCTION(Client, Reliable)
	void ClientCrouchStart(); // Ŭ�󸮾�Ʈ �ɱ� ���� (������ ȣ��Ǹ� �ڵ����� ȣ��)
	UFUNCTION(Client, Reliable)
	void ClientCrouchEnd(); // Ŭ�󸮾�Ʈ �ɱ� �� (������ ȣ��Ǹ� �ڵ����� ȣ��)
	UFUNCTION()
	void OnRep_IsCrouching(); // isCrouch ������ ����Ǹ� ȣ��Ǵ� �Լ� (�� �Լ����� ����Ǵ� �͵��� Ŭ���̾�Ʈ �ʿ��� ����Ǵ� �͵�)
	// --------------------------------------------------------------------------------------------


	//------------------------------------------- Roll ��Ÿ�� ��� ---------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool isDodge; // �������� ����
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMontageOnDodge(float Forward, float Right); // �������� ������ ��Ÿ�ָ� �����ϴ� �Լ� (��Ʈ�ѷ����� ȣ��)
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastMontage_Dodge(float Forward, float Right); // ������ ��Ÿ�� ��Ƽĳ��Ʈ (������ ȣ��Ǹ� �ڵ����� ȣ��)
	void SetDodgeMontage(float Forward, float Right);
	//---------------------------------------------------------------------------


	//------------------------------------------- ZoomIn ��� ---------------------------------
	UPROPERTY()
	FTimeline ZoomInTimeline;  // Timeline�� ������ ����
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* ZoomInCurve; // Ŀ�� ���� (�������Ʈ�� Ŀ�� ������)
	UFUNCTION()
	void ZoomInUpdate(float Alpha); // Ÿ�Ӷ����� ���۵� �� ȣ��� �Լ�
	UFUNCTION()
	void ZoomInFinished(); // Ÿ�Ӷ����� ������ �� ȣ��� �Լ�
	bool isZoomIn;
	void SetZoomIn(); // ī�޶� ���� ��ȯ (��Ʈ�ѷ����� ȣ��)
	//---------------------------------------------------------------------------------------------


	void SetCamera(); // ī�޶� ���� ��ȯ : 1��Ī <--> 3��Ī (��Ʈ�ѷ����� ȣ��)

	//------------------ Weapon Funtion ---------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool isReload;
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMontageOnReload(); // �������� ������ ��Ÿ�ָ� �����ϴ� �Լ� (��Ʈ�ѷ����� ȣ��)
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastMontage_Reload(); // ������ ��Ÿ�� ��Ƽĳ��Ʈ (������ ȣ��Ǹ� �ڵ����� ȣ��)
	void FinishReload();
	//---------------------------------------------------------------------

	

	//------------------ Weapon Funtion ---------------------------------------
	void Aiming();
	void StopAiming();

	UPROPERTY(ReplicatedUsing = OnRep_IsCrouching, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isFire = false;

	void Fire();
	void StopFire();
	void Reload();
	void Interact(); // 상호작용 함수
	void SwitchWeapon(TSubclassOf<AGun> NewGunClass);

	bool GetIsCrouching() const;
	bool GetIsSprinting() const;

	DECLARE_DELEGATE_OneParam(FDele_Player_Aimrate, float);
	FDele_Player_Aimrate func_Player_Aimrate;

	// ------------------------------------- Arrow Skill ----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<class AGnuProjectileActor> ArrowClass;
	void SpawnArrow();
	// -----------------------------------------------------------------------------------------
	

	// ------------------------------------- Arrow Skill ----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<class AGnuHealActor> HealClass;
	void SpawnHeal();
	// -----------------------------------------------------------------------------------------


	// ------------------------------------- HP, Stamina ----------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_CurHP, BlueprintReadWrite, Category = "Status")
	float CurHP;
	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float MaxHP;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float CurStamina;
	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float MaxStamina;

	UFUNCTION()
	void OnRep_CurHP();
	void UpdateHealth(float NewHP);
	void UpdateStamina(float NewStamina);
	void UpdateUIHealthAndStamina();

	// ��������Ʈ ���� �ִ� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGnuCharacterBaseWidget> CharacterHealthWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* CharacterWidget;

	UGnuCharacterBaseWidget* CharacterHealthWidget;

	// ------------------------------------------------------------------------

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom; // ī�޶� ��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FPPCamera; // 1��Ī ī�޶�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* TPPCamera; // 3��Ī ī�޶�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool isFPPCamera; // 1��Ī���� ����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float DefaultSpeed; // �⺻ �ӵ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed; // ������Ʈ �ӵ�


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DodgeMontage; // ����� ������ ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_F_Montage; // ������ ������ ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_B_Montage;  // �ڷ� ������ ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_R_Montage;  // ���������� ������ ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_L_Montage; // �������� ������ ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* Reload_Montage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	
	UPROPERTY()
	AGun* Gun; // Gun Actor

	TSubclassOf<UUserWidget> CrossHairWidgetClass;

	UPROPERTY()
	class UCrossHair* pCrossHair; // CrossHair UI
};
