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

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	AGnuMyPlayerController();

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
	TObjectPtr<UInputAction> WeaponChangeAction; // 무기 전환 (현재 q로 차례대로 바뀌게 설정해둠)

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> PistolChangeAction; // 무기 전환을 1,2,3번 누르면 바뀌게 하려면 이걸로 바꿔야 할 듯

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ZoomInAction; // 카메라 줌

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ArrowSkillAction; // 카메라 줌

	FVector2D CurrentMoveDirection; // 현재 이동 방향 Dodge할 때 사용


	void Move(const struct FInputActionValue& InputActionValue); // 이동
	void Rotation(const struct FInputActionValue& InputActionValue); // 회전
	void Dodge(const struct FInputActionValue& InputActionValue); // 구르기
	void Sprint(const struct FInputActionValue& InputActionValue); // 달리기
	void SprintStop(const struct FInputActionValue& InputActionValue); // 달리기 끝
	void ToggleCrouch(const struct FInputActionValue& InputActionValue); // 앉기
	void ToggleCamera(const struct FInputActionValue& InputActionValue); // 카메라 인칭 전환
	void ToggleZoomIn(const struct FInputActionValue& InputActionValue); // 카메라 줌인
	void PistolChange(const struct FInputActionValue& InputActionValue); // 권총으로 체인지
	void WeaponChange(const struct FInputActionValue& InputActionValue); // 무기 체인지 (PistolChange, RifleChange이 것들 만들어서 할꺼면 삭제 해야함)
	void Jump(const struct FInputActionValue& InputActionValue); // 점프
	void StopJumping(const struct FInputActionValue& InputActionValue); // 점프 끝
	void ArrowSkill(const struct FInputActionValue& InputActionValue); // 에로우 스킬
};