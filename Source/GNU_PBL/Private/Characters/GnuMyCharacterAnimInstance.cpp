// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacterAnimInstance.h"
#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCurrentAcceleration(), CharacterMovement->Velocity
#include "Kismet/KismetMathLibrary.h" // VSizeXY(), NormalizeAxis(), NormalizedDeltaRotator()
#include "Net/UnrealNetwork.h"

void UGnuMyCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyCharacter = Cast<AGnuMyCharacter>(TryGetPawnOwner());
	if (MyCharacter != nullptr)
	{
		MyCharacterMovement = MyCharacter->GetCharacterMovement();
	}

	CurDirectionAngle = 0.0f;
	isSprinting = false;
	isCrouching = false;
	EAnimState = EAnimationState::Unarmed; // 맨 손으로 초기화
}

void UGnuMyCharacterAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 변수를 네트워크에서 복제할 수 있도록 설정
	DOREPLIFETIME(UGnuMyCharacterAnimInstance, EAnimState);
}

void UGnuMyCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (MyCharacter != nullptr && MyCharacter->IsValidLowLevel())
    {
		IsFalling = MyCharacterMovement->IsFalling();
		isSprinting = MyCharacter->isSprint;
		isCrouching = MyCharacter->isCrouch;

		//------------------------------ ShouldMove ----------------------------
		// 현재 움직이고 있는지 확인
		CurVelocity = MyCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(CurVelocity);

		CurAcceleration = MyCharacterMovement->GetCurrentAcceleration();
		ShouldMove = (GroundSpeed > 3.0f) && !CurAcceleration.IsZero();
		//------------------------------------------------------------------------

		// ------------------------------ Direction -----------------------------
		float XValue = CurVelocity.X;
		float YValue = CurVelocity.Y;

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
		if (UKismetMathLibrary::InRange_FloatFloat(Direction, -70.0f, 70.0f, true, true))
		{
			MovementInput = EMovementInput::Forward;
		}
		else
		{
			if (UKismetMathLibrary::InRange_FloatFloat(Direction, 70.0f, 110.0f, true, true))
			{
				MovementInput = EMovementInput::Right;
			}
			else
			{
				if (UKismetMathLibrary::InRange_FloatFloat(Direction, -110.0f, -70, true, true))
				{
					MovementInput = EMovementInput::Left;
				}
				else
				{
					MovementInput = EMovementInput::Backward;
				}
			}
		}
		//------------------------------------------------------------------------

		// ------------------------- Aim Offset : 고개 돌리는 기능에 사용 -----------------------
		// Yaw, Picth 값을 사용해 idle 상태에서 고개 회전 
		CurPitch = UKismetMathLibrary::NormalizedDeltaRotator(MyCharacter->GetBaseAimRotation(), MyCharacter->GetActorRotation()).Pitch;
		//------------------------------------------------------------------------

		// --------------------------블렌드 스페이스 1D Lean : 뛰는 방향에 맞게 몸이 틀어지는 기능에 사용 ---------------------------
		CurDirectionAngle = UKismetMathLibrary::FInterpTo(CurDirectionAngle, TurnRate, GetWorld()->GetDeltaSeconds(), 10.0f);
		//-------------------------------------------------------------------------------------------------------

	}

}

void UGnuMyCharacterAnimInstance::SetTurnRate(float CurYaw)
{
	TurnRate = CurYaw;
}


void UGnuMyCharacterAnimInstance::SetWeapon()
{
	if (EAnimState == EAnimationState::Unarmed)
	{
		EAnimState = EAnimationState::Pistol;
	}
	else if (EAnimState == EAnimationState::Pistol)
	{
		EAnimState = EAnimationState::Rifle;
	}
	else if (EAnimState == EAnimationState::Rifle)
	{
		EAnimState = EAnimationState::Unarmed;
	}

	// 디버그 메시지 출력
	if (GEngine) 
	{
		FString s = FString::Printf(TEXT("SetWeapon called: %d"), (int32)EAnimState);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, s);
	}
}

void UGnuMyCharacterAnimInstance::MultiCastSetWeapon_Implementation()
{
	SetWeapon();
	// 디버그 메시지 출력
	if (GEngine)
	{

		FString s = FString::Printf(TEXT("MulticastSetWeapon called: %d"), (int32)EAnimState);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, s);
	}
}

void UGnuMyCharacterAnimInstance::ServerSetWeapon_Implementation() 
{
	SetWeapon();
	if (GEngine)
	{

		FString s = FString::Printf(TEXT("ServerSetWeapon called: %d"), (int32)EAnimState);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, s);
	}
	MultiCastSetWeapon();
}

bool UGnuMyCharacterAnimInstance::ServerSetWeapon_Validate()
{
	return true;
}

// 현재 Yaw 값을 가져와서 YawLastTick과의 차이를 계산
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
