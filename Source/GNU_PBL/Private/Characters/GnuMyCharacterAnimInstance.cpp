// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacterAnimInstance.h"
#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCurrentAcceleration(), CharacterMovement->Velocity
#include "Kismet/KismetMathLibrary.h" // VSizeXY(), NormalizeAxis

void UGnuMyCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyCharacter = Cast<AGnuMyCharacter>(TryGetPawnOwner());
	if (MyCharacter != nullptr)
	{
		MyCharacterMovement = MyCharacter->GetCharacterMovement();
	}

	isSprinting = false;
	isCrouching = false;
}

void UGnuMyCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (MyCharacter != nullptr && MyCharacter->IsValidLowLevel())
    {
		IsFalling = MyCharacterMovement->IsFalling();

		//------------------------------ ShouldMove ----------------------------
		// 현재 움직이고 있는지 확인
		Velocity = MyCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		FVector CurrentAcceleration = MyCharacterMovement->GetCurrentAcceleration();
		ShouldMove = (GroundSpeed > 3.0f) && !CurrentAcceleration.IsZero();
		//------------------------------------------------------------------------


		// ------------------------------ Direction -----------------------------
		float XValue = Velocity.X;
		float YValue = Velocity.Y;

		// X와 Y를 사용하여 새로운 FVector를 만듭니다. (Z는 0으로 설정)
		FVector NewVelocity = FVector(XValue, YValue, 0.0f);

		// 캐릭터의 현재 회전 값을 저장합니다.
		FRotator CurrentRotation = MyCharacter->GetActorRotation();

		// CalculateDirection은 FVector와 FRotator를 입력으로 받고, 이동 방향을 반환합니다.
		float DirectionAngle = UAnimInstance::CalculateDirection(NewVelocity, CurrentRotation);
		Direction = UKismetMathLibrary::NormalizeAxis(DirectionAngle);
		//------------------------------------------------------------------------


		// ------------------------------ Orientation -----------------------------
		// Angle값을 기준으로 애니메이션 그래프에서 본의 위치를 자연스럽게 변화시키면서 부드러운 애니메이션 출력
		F_Orientation_Angle = Direction;
		R_Orientation_Angle = Direction - 90.0f;
		B_Orientation_Angle = Direction - 180.0f;
		L_Orientation_Angle = Direction + 90.0f;
		//------------------------------------------------------------------------


		// ------------------------------ MovementInput(Direction Angles) -----------------------------
		// enum을 사용해 현재 캐릭터가 바라보는 방향에 맞게 세팅
		// 이 값들로 애니메이션 그래프 BlnedPoses를 사용해 각 방향에 맞는 애니메이션 출력
		if (Direction > -45.0f && Direction <= 45.0f)
		{
			MovementInput = EMovementInput::Forward;
		}
		else if (Direction > 45.0f && Direction <= 135.0f)
		{
			MovementInput = EMovementInput::Right;
		}
		else if (Direction > 135.0f || Direction <= -135.0f)
		{
			MovementInput = EMovementInput::Backward;
		}
		else if (Direction > -135.0f && Direction <= -45.0f)
		{
			MovementInput = EMovementInput::Left;
		}
		//------------------------------------------------------------------------

		//   // 현재 Yaw 값을 가져와서 YawLastTick과의 차이를 계산
		//Yaw = MyCharacter->GetActorRotation().Yaw;
		//YawLastTick = Yaw;
		//// YawLastTick을 현재 Yaw로 업데이트
		//YawChangeOver = Yaw - YawLastTick;

		//if (ShouldMove || IsFalling)
		//{
		//	YawOffset = 0.0f;
		//}
		//else
		//{
		//	YawOffset = UKismetMathLibrary::NormalizeAxis(YawChangeOver + YawOffset);
		//	// CurveValue를 가져오고 NearlyEqual을 사용하여 조건 확인
		//	float IsTurnCurveValue = GetCurveValue(FName(TEXT("IsTurn")));
		//	float DistanceCurveValue = GetCurveValue(FName(TEXT("DistanceCurve")));
		//	if (UKismetMathLibrary::NearlyEqual_FloatFloat(IsTurnCurveValue, 1.0f, 0.001f))
		//	{
		//		// DoOnce의 Completed 핀에 연결
		//		DoOnce = false;
		//		CurveValue = DistanceCurveValue;

		//		LastCurveValue = CurveValue;
		//		CurveValue = DistanceCurveValue;

		//		YawOffset = YawOffset - ((LastCurveValue - CurveValue) * UKismetMathLibrary::SelectFloat(-1.0f, 1.0f, YawOffset > 0.0f));
		//	}
		//	else
		//	{
		//		DoOnce = true;
		//	}
		//}
	}

}

void UGnuMyCharacterAnimInstance::SwitchSprint()
{
	isSprinting = !isSprinting;
}

void UGnuMyCharacterAnimInstance::SwitchCrouch()
{
	isCrouching = !isCrouching;
}

bool UGnuMyCharacterAnimInstance::GetIsSprinting()
{
	return isSprinting;
}

bool UGnuMyCharacterAnimInstance::GetIsCroucing()
{
	return isCrouching;
}
