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
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ToggleCameraAction;

	FVector2D CurrentMoveDirection; // Dodge할 때 사용

	void Move(const struct FInputActionValue& InputActionValue);
	void Rotation(const struct FInputActionValue& InputActionValue);
	void Dodge(const struct FInputActionValue& InputActionValue);
	void ToggleSprint(const struct FInputActionValue& InputActionValue);
	void ToggleCrouch(const struct FInputActionValue& InputActionValue);
	void ToggleCamera(const struct FInputActionValue& InputActionValue);
	void Jump(const struct FInputActionValue& InputActionValue);
	void StopJumping(const struct FInputActionValue& InputActionValue);
};

// TObjectPtr :  스마트 포인터 타입으로, 주로 UObject 기반 객체를 안전하게 관리하는 데 사용
// Unreal Engine의 가비지 컬렉션 시스템과 통합되어, 객체가 더 이상 필요하지 않을 때 자동으로 메모리를 해제할 수 있도록 돕습니다. 
// -> 가비지 컬렉터 : 객체의 참조 카운트를 추적하여, 더 이상 참조되지 않는 객체를 자동으로 청소
// 특정 UObject 타입만 가리킬 수 있도록 제한합니다. 이는 다른 타입의 객체를 잘못 참조하는 오류를 방지, 컴파일 타임에서 문제를 미리 감지
// -> 예를 들어, UInputAction 타입의 객체가 아닌 다른 객체를 할당하려고 하면 컴파일 오류가 발생