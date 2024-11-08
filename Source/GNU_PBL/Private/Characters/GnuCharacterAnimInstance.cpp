// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuCharacterAnimInstance.h"
#include "Characters/GnuCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UGnuCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyCharacter = Cast<AGnuCharacter>(TryGetPawnOwner());
	if (MyCharacter != nullptr)
	{
		MyCharacterMovement = MyCharacter->GetCharacterMovement();
	}
}

void UGnuCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (MyCharacterMovement != nullptr)
	{
		MoveLength = UKismetMathLibrary::VSizeXY(MyCharacterMovement->Velocity);
		isJump = MyCharacterMovement->IsFalling();
	}
}

void UGnuCharacterAnimInstance::SetIsStraightMove(bool Value)
{
	isStraightMove = Value;
}

void UGnuCharacterAnimInstance::SetIsBackMove(bool Value)
{
	isBackMove = Value;
}

void UGnuCharacterAnimInstance::SetIsLeftMove(bool Value)
{
	isLeftMove = Value;
}

void UGnuCharacterAnimInstance::SetIsRightMove(bool Value)
{
	isRightMove = Value;
}

void UGnuCharacterAnimInstance::SetAimPitch(float Value)
{
	AimPitch = Value;
}

bool UGnuCharacterAnimInstance::isWalking()
{
	if (MoveLength == 0)
		return false;
	else return true;
}
