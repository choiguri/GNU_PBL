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

// Monster class
class AGnuAttackCollisionActor;

UCLASS()
class GNU_PBL_API AGnuCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	UGnuCharacterAnimInstance* MyAnimInstance;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* RotationAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	bool isWaking;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& value);
	void Rotation(const FInputActionValue& value);
	void UpdateAnimInstance(const FVector2D& MoveVector2D);

	// 공격 맞았는지 체크
	UPROPERTY(VisibleInstanceOnly)
	AGnuAttackCollisionActor* OverlapItem;

public:	
	// Sets default values for this character's properties
	AGnuCharacter();

	void SetOverlapItem(AGnuAttackCollisionActor* _overlapItem) { OverlapItem = _overlapItem; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
};
