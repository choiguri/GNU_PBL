// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GnuCharacterPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGnuCharacterPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;



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
	TObjectPtr<UInputAction> WeaponChangeAction; // 무기 전환 (현재 q로 차례대로 바뀌게 설정해둠)
	
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> PistolChangeAction; // 무기 전환을 1,2,3번 누르면 바뀌게 하려면 이걸로 바꿔야 할 듯

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ZoomInAction; // 카메라 줌

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
};

// TObjectPtr :  스마트 포인터 타입으로, 주로 UObject 기반 객체를 안전하게 관리하는 데 사용
// Unreal Engine의 가비지 컬렉션 시스템과 통합되어, 객체가 더 이상 필요하지 않을 때 자동으로 메모리를 해제할 수 있도록 돕습니다. 
// -> 가비지 컬렉터 : 객체의 참조 카운트를 추적하여, 더 이상 참조되지 않는 객체를 자동으로 청소
// 특정 UObject 타입만 가리킬 수 있도록 제한합니다. 이는 다른 타입의 객체를 잘못 참조하는 오류를 방지, 컴파일 타임에서 문제를 미리 감지
// -> 예를 들어, UInputAction 타입의 객체가 아닌 다른 객체를 할당하려고 하면 컴파일 오류가 발생