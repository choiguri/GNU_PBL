// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyAnimInstance.h"
#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCurrentAcceleration(), CharacterMovement->Velocity
#include "Kismet/KismetMathLibrary.h" // VSizeXY(), NormalizeAxis(), NormalizedDeltaRotator()
#include "Net/UnrealNetwork.h"
#include "KismetAnimationLibrary.h"
#include "Weapons/GnuWeapon.h"




void UGnuMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyCharacter = Cast<AGnuMyCharacter>(TryGetPawnOwner());
	if (MyCharacter != nullptr)
	{
		MyCharacterMovement = MyCharacter->GetCharacterMovement();
	}

	ShouldMove = false;
	IsFalling = false;
	GroundSpeed = 0.0f;
	CurDirectionAngle = 0.0f;
	Direction = 0.0f;
	F_Orientation_Angle = 0.0f;
	B_Orientation_Angle = 0.0f;
	R_Orientation_Angle = 0.0f;
	L_Orientation_Angle = 0.0f;
	CurPitch = 0.0f;
	TurnRate = 0.0f;
	CurDirectionAngle = 0.0f;
	CurVelocity = FVector::Zero();
	CurAcceleration = FVector::Zero();
	EAnimState = EAnimationState::Unarmed; // 시작했을 때 기본 자세
	bIsCrouching = false;
	bIsSprinting = false;
	bIsCrouched = false; // 나중에 지워야할 듯
	
}

void UGnuMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MyCharacter != nullptr && MyCharacter->IsValidLowLevel())
	{
		IsFalling = MyCharacterMovement->IsFalling();
		UpdateMovementState();
		UpdateDirectionAndMovementInput();
		bIsCrouching = MyCharacter->GetIsCrouching();
		bIsSprinting = MyCharacter->GetIsSprinting();
		bWeaponEquipped = MyCharacter->IsWeaponEquipped();
		EquippedWeapon = MyCharacter->GetEquippedWeapon();
		bIsCrouched = MyCharacter->bIsCrouched;
		SetWeapon();
		
	}

}

void UGnuMyAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGnuMyAnimInstance, EAnimState);
}

void UGnuMyAnimInstance::UpdateMovementState()
{
	//------------------------------ ShouldMove 확인 ----------------------------
	// 현재 캐릭터의 속도와 가속도를 사용하여 캐릭터가 움직이는지 확인
	CurVelocity = MyCharacterMovement->Velocity;
	GroundSpeed = UKismetMathLibrary::VSizeXY(CurVelocity);

	CurAcceleration = MyCharacterMovement->GetCurrentAcceleration();
	ShouldMove = (GroundSpeed > 3.0f) && !CurAcceleration.IsZero();
	//------------------------------------------------------------------------
}

void UGnuMyAnimInstance::UpdateDirectionAndMovementInput()
{
	// ------------------------------ Direction 계산 -----------------------------
	// 캐릭터의 속도 벡터를 사용하여 현재 이동 방향을 계산

	// X와 Y를 사용하여 새로운 FVector를 만듭니다. (Z는 0으로 설정)
	FVector NewVelocity = FVector(CurVelocity.X, CurVelocity.Y, 0.0f);

	// 캐릭터의 현재 회전 값을 저장합니다.
	FRotator CurrentRotation = MyCharacter->GetActorRotation();

	// CalculateDirection은 FVector와 FRotator를 입력으로 받고, 이동 방향을 반환합니다.
	float DirectionAngle = UKismetAnimationLibrary::CalculateDirection(NewVelocity, CurrentRotation);

	Direction = UKismetMathLibrary::NormalizeAxis(DirectionAngle);
	//------------------------------------------------------------------------


	// ------------------------------ Orientation (방향별 각도 설정) -----------------------------
	// 캐릭터의 방향을 기준으로 각각의 방향에 따른 각도를 설정
	// Orientation_Angle 값을 기준으로 애니메이션 그래프에서 본의 위치를 자연스럽게 변화시키면서 부드러운 애니메이션 출력
	F_Orientation_Angle = Direction;
	R_Orientation_Angle = Direction - 90.0f;
	B_Orientation_Angle = Direction - 180.0f;
	L_Orientation_Angle = Direction + 90.0f;
	//------------------------------------------------------------------------


	// ------------------------------ MovementInput(Direction Angles) : 이동 방향에 따른 처리-----------------------------
	// enum을 사용해 현재 캐릭터가 바라보는 방향에 맞게 세팅
	// 이 값들로 애니메이션 그래프 BlendPoses(EAnimationState)를 사용해 각 방향에 맞는 애니메이션 출력
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
	//--------------------------------------------------------------------------------------------------


	// ------------------------------ Aim Offset 처리 ------------------------------
	// 캐릭터의 Yaw와 Pitch 값을 계산하여 idle 상태에서 고개를 돌리는 애니메이션 처리
	// Yaw값은 애니메이션 이벤트 그래프에서 처리 (c++이식 실패)
	CurPitch = UKismetMathLibrary::NormalizedDeltaRotator(MyCharacter->GetBaseAimRotation(), MyCharacter->GetActorRotation()).Pitch;
	//---------------------------------------------------------------------------

	// -------------------------- 블렌드 스페이스 1D Lean ------------------------------
	// 뛰는 방향에 맞게 몸이 틀어지는 기능에 사용
	CurDirectionAngle = UKismetMathLibrary::FInterpTo(CurDirectionAngle, TurnRate, GetWorld()->GetDeltaSeconds(), 10.0f);
	//-------------------------------------------------------------------------------------------------------
}

void UGnuMyAnimInstance::SetTurnRate(float CurYaw) // GnuCharacterPlayerController에서 호출
{
	// TurnRate : 마우스로 카메라를 회전할 때 값이 변동됨
	TurnRate = CurYaw;
}

// 추후에 무기를 늘리거나 하면 수정해야 할 듯
void UGnuMyAnimInstance::SetWeapon()
{

	if (bWeaponEquipped)
	{
		EAnimState = EAnimationState::Rifle;
	}
	else if (!bWeaponEquipped)
	{
		EAnimState = EAnimationState::Unarmed;
	}
}

void UGnuMyAnimInstance::ServerSetAnimState_Implementation() // Ŭ���̾�Ʈ���� ������Ʈ ���� ��û�� ������ �������� ServerSprintStart_Implementation�� ����
{
	SetWeapon(); //  �� �Լ��� ���������� ����
	if (GEngine)
	{
		FString s = FString::Printf(TEXT("Server SetWeapon called: %d"), (int32)EAnimState);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, s);
	}

}

bool UGnuMyAnimInstance::ServerSetAnimState_Validate()
{
	return true; // �ʿ��� ��ȿ�� �˻� ������ ���⿡ �߰�
}


void UGnuMyAnimInstance::ClientSetAnimState_Implementation() // Ŭ���̾�Ʈ�� ��� �ݿ��ϴ� ��������� ���� - Ŭ���̾�Ʈ �ϰ����� ������
{
	// �ּ��� Ǯ�� Ŭ���̾�Ʈ���� ������Ʈ ���¸� ������ �ݿ��� �� ������, �� ����� ������ ����� ��ٸ��� �ʱ� ������ ������ Ŭ���̾�Ʈ�� �ϰ����� ������ �� ����
	// UpdateSprintState(true);
}


void UGnuMyAnimInstance::OnRep_SetAnimState() 	// Ŭ���̾�Ʈ�� ����ȭ �Ǵ� �Լ� -> �������� isSprint ���� ����� �� Ŭ���̾�Ʈ���� �� ��ȭ�� �����ϰ� ����Ǵ� �Լ� (��, ������ �����ϴ� ������Ʈ ���¸� Ŭ���̾�Ʈ�� ����ȭ �ϴ� ���)
{
	SetWeapon();
	if (GEngine)
	{
		FString s = FString::Printf(TEXT("OnRep called: %d"), (int32)EAnimState);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, s);
	}

}
