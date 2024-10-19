// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GnuCharacter.generated.h"



// Input ����
class UInputMappingContext;
class UInputAction;
class UGnuCharacterAnimInstance;
// ��Ʈ�� ����
class USpringArmComponent;
class UCameraComponent;
class AGun;

UCLASS()
class GNU_PBL_API AGnuCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	UGnuCharacterAnimInstance* MyAnimInstance;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* RotationAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* AimingAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* ReroadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	bool isWaking;
	

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& value);
	void Rotation(const FInputActionValue& value);
	void UpdateAnimInstance(const FVector2D& MoveVector2D);
	void Aiming();
	void StopAiming();
	void StartFire();
	void StopFire();
	void Reroad();

public:	
	// Sets default values for this character's properties
	AGnuCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	DECLARE_DELEGATE_OneParam(FDele_Player_Aimrate, float);

	FDele_Player_Aimrate func_Player_Aimrate;

	UPROPERTY()
	class UCrossHair* pCrossHair;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	AGun* Gun;

	TSubclassOf<UUserWidget> CrossHairWidgetClass;
	UUserWidget* CrossHairWidget;
};
