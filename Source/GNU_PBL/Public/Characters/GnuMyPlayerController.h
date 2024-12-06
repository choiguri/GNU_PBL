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

	virtual void OnPossess(APawn* InPawn) override;

	// 추가사항
	void SetHUDHealth(float Health, float MaxHealth);

	void SetHUDStamina(float Stamina, float MaxStamina);

	void SetHUDCombatTime(float CombatTime);

	void SetHUDWeaponAmmo(int32 Ammo, int32 MaxAmmo);

	void SetHUDDeathCount(int32 Deathcount);

	/*void SetHUDOthersHealth(FString PlayerName, float Health, float MaxHealth, int32 Index);*/

	UFUNCTION(NetMulticast, Reliable)
	void SetHUDOthersHealth(const TArray<float>& HealthArray);
	UFUNCTION(NetMulticast, Reliable)
	void SetHUDOthersName(const TArray<FString>& NameArray);

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
	TObjectPtr<UInputAction> MoveAction; // 이동

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction; // 회전

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> JumpAction; // 점프

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SprintAction; // 달리기

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> CrouchAction; // 앉기

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> DodgeAction; // 구르기

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ToggleCameraAction; // 카메라 인칭 전환

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ZoomInAction; // 카메라 줌



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
	TObjectPtr<UInputAction> ArrowSkillAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> HealSkillAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> GrenadeSkillAction;

	FVector2D CurrentMoveDirection; // 현재 이동 방향 Dodge할 때 사용


	void Move(const struct FInputActionValue& InputActionValue); // 이동
	void Rotation(const struct FInputActionValue& InputActionValue); // 회전
	void Dodge(const struct FInputActionValue& InputActionValue); // 구르기
	void Sprint(const struct FInputActionValue& InputActionValue); // 달리기
	void SprintStop(const struct FInputActionValue& InputActionValue); // 달리기 끝
	void ToggleCrouch(const struct FInputActionValue& InputActionValue); // 앉기
	void ToggleCamera(const struct FInputActionValue& InputActionValue); // 카메라 인칭 전환
	void ToggleZoomIn(const struct FInputActionValue& InputActionValue); // 카메라 줌인
	void Jump(const struct FInputActionValue& InputActionValue); // 점프
	void StopJumping(const struct FInputActionValue& InputActionValue); // 점프 끝
	void ArrowSkill(const struct FInputActionValue& InputActionValue);
	void HealSkill(const struct FInputActionValue& InputActionValue);
	void GrenadeSkill(const struct FInputActionValue& InputActionValue);

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

// TObjectPtr :  스마트 포인터 타입으로, 주로 UObject 기반 객체를 안전하게 관리하는 데 사용
// Unreal Engine의 가비지 컬렉션 시스템과 통합되어, 객체가 더 이상 필요하지 않을 때 자동으로 메모리를 해제할 수 있도록 돕습니다. 
// -> 가비지 컬렉터 : 객체의 참조 카운트를 추적하여, 더 이상 참조되지 않는 객체를 자동으로 청소
// 특정 UObject 타입만 가리킬 수 있도록 제한합니다. 이는 다른 타입의 객체를 잘못 참조하는 오류를 방지, 컴파일 타임에서 문제를 미리 감지
// -> 예를 들어, UInputAction 타입의 객체가 아닌 다른 객체를 할당하려고 하면 컴파일 오류가 발생