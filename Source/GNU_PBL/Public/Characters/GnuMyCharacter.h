// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GnuBaseCharacter.h"
#include "Components/TimelineComponent.h" // FTimeline
#include "GameFramework/PlayerState.h"
#include "GnuMyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class AGun;
class UFTimeline;
class UCurveFloat;
class AGnuProjectileActor;
class AGnuHealActor;
class AGnuGrenadeActor;
class UGnuCharacterBaseWidget;

// Monster class
class AGnuAttackCollisionActor;

UCLASS()
class GNU_PBL_API AGnuMyCharacter : public AGnuBaseCharacter
{
	GENERATED_BODY()

public:
	AGnuMyCharacter();

	void SetOverlapItem(AGnuAttackCollisionActor* _overlapItem) { OverlapItem = _overlapItem; }

	// --------------------------------------------- Sprint 기능 -----------------------------------
// ReplicatedUsing = OnRep_IsSprinting : 이렇게 선언하면 값이 변동될 때마다 자동으로 OnRep_IsSprinting함수가 호출된다.
	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isSprint;  // 달리는지 여부 (GnuMyCharacterAnimInstance에서 사용 중)
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintStart(); // 서버 달리기 시작 (컨트롤러에서 호출)
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintEnd(); // 서버 달리기 끝 (컨트롤러에서 호출)
	UFUNCTION(Client, Reliable)
	void ClientSprintStart(); // 클라이언트 달리기 시작 (서버가 호출되면 자동으로 호출)
	UFUNCTION(Client, Reliable)
	void ClientSprintEnd(); // 클라이어트 달리기 끝 (서버가 호출되면 자동으로 호출)
	UFUNCTION()
	void OnRep_IsSprinting(); // isSprint 변수가 변경되면 호출되는 함수 (이 함수에서 실행되는 것들이 클라이언트 쪽에서 실행되는 것들)
	void UpdateSprintState(bool bIsSprinting);
	// --------------------------------------------------------------------------------------------


	// --------------------------------------------- Crouch 기능 ------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_IsCrouching, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool isCrouch;  // 달리는지 여부
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrouchStart(); // 서버 앉기 시작 (컨트롤러에서 호출)
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrouchEnd(); // 서버 앉기 끝 (컨트롤러에서 호출)
	UFUNCTION(Client, Reliable)
	void ClientCrouchStart(); // 클라리언트 앉기 시작 (서버가 호출되면 자동으로 호출)
	UFUNCTION(Client, Reliable)
	void ClientCrouchEnd(); // 클라리언트 앉기 끝 (서버가 호출되면 자동으로 호출)
	UFUNCTION()
	void OnRep_IsCrouching(); // isCrouch 변수가 변경되면 호출되는 함수 (이 함수에서 실행되는 것들이 클라이언트 쪽에서 실행되는 것들)
	// --------------------------------------------------------------------------------------------


	//------------------------------------------- Roll 몽타주 기능 ---------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool isDodge; // 구르는지 여부
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMontageOnDodge(float Forward, float Right); // 서버에서 구르기 몽타주를 실행하는 함수 (컨트롤러에서 호출)
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastMontage_Dodge(float Forward, float Right); // 구르기 몽타주 멀티캐스트 (서버가 호출되면 자동으로 호출)
	void SetDodgeMontage(float Forward, float Right);
	//---------------------------------------------------------------------------


	//------------------------------------------- ZoomIn 기능 ---------------------------------
	bool isZoomIn;
	void SetZoomIn(); // 카메라 시점 전환 (컨트롤러에서 호출)

	UPROPERTY()
	FTimeline ZoomInTimeline;  // Timeline을 저장할 변수

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* ZoomInCurve; // 커브 변수 (블루프린트로 커브 만들어둠)

	UFUNCTION()
	void ZoomInUpdate(float Alpha); // 타임라인이 시작될 때 호출될 함수

	UFUNCTION()
	void ZoomInFinished(); // 타임라인이 끝났을 때 호출될 함수
	//---------------------------------------------------------------------------------------------


	//------------------ Weapon Funtion ---------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool isReload;
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMontageOnReload();
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastMontage_Reload();
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
	

	// ------------------------------------- Heal Skill ----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<class AGnuHealActor> HealClass;
	void SpawnHeal();
	// -----------------------------------------------------------------------------------------

	// ------------------------------------- Grenade Skill ----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<class AGnuGrenadeActor> GrenadeClass;
	void SpawnGrenade();
	// -----------------------------------------------------------------------------------------

	void SetCamera();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom; // 카메라 암

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FPPCamera; // 1인칭 카메라

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* TPPCamera; // 3인칭 카메라

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	bool isFPPCamera;// 1인칭인지 여부

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float DefaultSpeed; // 기본 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;  // 스프린트 속도


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DodgeMontage; // 저장된 구르기 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_F_Montage; // 앞으로 구르기 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_B_Montage;  // 뒤로 구르기 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_R_Montage;  // 오른쪽으로 구르기 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DiveRoll_L_Montage; // 왼쪽으로 구르기 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* Reload_Montage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	
	UPROPERTY()
	AGun* Gun; // Gun Actor


	TSubclassOf<UUserWidget> CrossHairWidgetClass;

	UPROPERTY()
	class UCrossHair* pCrossHair; // CrossHair UI

	// 공격 맞았는지 체크
	UPROPERTY(VisibleInstanceOnly)
	AGnuAttackCollisionActor* OverlapItem;



// Character 합치면서 작성
private:
	// 머리 위의 스팀 닉네임 표시
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverHeadWidget;

	// HP바 설정
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 1.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxStaminaa = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina, VisibleAnywhere, Category = "Player Stats")
	float Stamina = 1.f;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_Stamina();

	class AGnuMyPlayerController* GNUPlayerController;

	UPROPERTY(EditAnywhere, Category = "Player Name")
	FString LocalPlayerName = TEXT("Unknown Player");


public:
	// 서버에서 수정 -> RepNotify -> 클라이언트 반응
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Implementation으로 정의해서 밑줄이 뜨더라도 오류가 아님
	UFUNCTION(Client, Reliable)
	void ClientSetName(const FString& Name);

	UFUNCTION(Server, Reliable)
	void ServerSetPlayerName(const FString& PlayerName);

	//
	// GNUGameMode와 관련
	//
	void Elim();

protected:
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	
	void UpdateHUDHealth();
	void UpdateHUDStamina();
};
