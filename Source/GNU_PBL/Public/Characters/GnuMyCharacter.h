// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GnuBaseCharacter.h"
#include "Components/TimelineComponent.h" // FTimeline

#include "GnuMyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFTimeline;
class UCurveFloat;

class AGnuProjectileActor;
class UGnuCharacterBaseWidget;

UCLASS()
class GNU_PBL_API AGnuMyCharacter : public AGnuBaseCharacter
{
	GENERATED_BODY()

private:
	// 네트워크 복제를 위한 함수 선언 : UFUNCTION(Server) 기능 사용하려면 이 함수가 있어야 함
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	AGnuMyCharacter();

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


	void SetCamera(); // 카메라 시점 전환 : 1인칭 <--> 3인칭 (컨트롤러에서 호출)

	// ------------------------------------- Arrow Skill ----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<class AGnuProjectileActor> ArrowClass;

	void SpawnArrow();
	// -----------------------------------------------------------------------------------------

	// ------------------------------------- HP, Stamina ----------------------------------------
	UPROPERTY(ReplicatedUsing = OnRep_CurHP, BlueprintReadWrite, Category = "Status")
	float CurHP;
	UPROPERTY(BlueprintReadWrite, Category = "Status")
	float MaxHP;

	UFUNCTION()
	void OnRep_CurHP();

	// Update functions
	void UpdateHealth(float NewHP);

	// UI Update function (called in BeginPlay)
	void UpdateUIHealthAndStamina();

	// 블루프린트 위젯 넣는 곳
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGnuCharacterBaseWidget> CharacterHealthWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* CharacterWidget;

	UGnuCharacterBaseWidget* CharacterHealthWidget;

	// ------------------------------------------------------------------------
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	//TSubclassOf<class AGnuHPActor> HPClass;

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
	bool isFPPCamera; // 1인칭인지 여부

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float DefaultSpeed; // 기본 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed; // 스프린트 속도


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
};
