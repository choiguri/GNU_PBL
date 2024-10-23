// Fill out your copyright notice in the Description page of Project Settings.

//GnuCharacterAnimInstance.cpp


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

bool UGnuCharacterAnimInstance::isWalking()
{
	if (MoveLength == 0)
		return false;
	else return true;
}
bool UGnuCharacterAnimInstance::isRolling()
{
	isRoll = !isRoll;
	return isRoll;
}
void UGnuCharacterAnimInstance::PlayRollMontage()
{
	// RollMontage가 설정되어 있고, 현재 다른 몽타주가 재생 중이 아닐 때만 구르기 몽타주 재생
	if (RollMontage && !Montage_IsPlaying(nullptr))
	{
		
		Montage_Play(RollMontage);
	}
	else if (RollMontage == nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("There's no Montage")));
		}
	}
}

// 현재 몽타주가 재생 중인지 확인하는 함수 구현
bool UGnuCharacterAnimInstance::IsPlayingAnyMontage() const
{
	return Montage_IsPlaying(nullptr); // 모든 몽타주에 대해 재생 여부를 확인
}