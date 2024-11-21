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

void UGnuMyAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ������ ��Ʈ��ũ���� ������ �� �ֵ��� ����
	DOREPLIFETIME(UGnuMyAnimInstance, EAnimState);
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

void UGnuMyAnimInstance::UpdateMovementState()
{
	//------------------------------ ShouldMove Ȯ�� ----------------------------
	// ���� ĳ������ �ӵ��� ���ӵ��� ����Ͽ� ĳ���Ͱ� �����̴��� Ȯ��
	CurVelocity = MyCharacterMovement->Velocity;
	GroundSpeed = UKismetMathLibrary::VSizeXY(CurVelocity);

	CurAcceleration = MyCharacterMovement->GetCurrentAcceleration();
	ShouldMove = (GroundSpeed > 3.0f) && !CurAcceleration.IsZero();
	//------------------------------------------------------------------------
}

void UGnuMyAnimInstance::UpdateDirectionAndMovementInput()
{
	// ------------------------------ Direction ��� -----------------------------
	// ĳ������ �ӵ� ���͸� ����Ͽ� ���� �̵� ������ ���

	// X�� Y�� ����Ͽ� ���ο� FVector�� ����ϴ�. (Z�� 0���� ����)
	FVector NewVelocity = FVector(CurVelocity.X, CurVelocity.Y, 0.0f);

	// ĳ������ ���� ȸ�� ���� �����մϴ�.
	FRotator CurrentRotation = MyCharacter->GetActorRotation();

	// CalculateDirection�� FVector�� FRotator�� �Է����� �ް�, �̵� ������ ��ȯ�մϴ�.
	float DirectionAngle = UKismetAnimationLibrary::CalculateDirection(NewVelocity, CurrentRotation);

	Direction = UKismetMathLibrary::NormalizeAxis(DirectionAngle);
	//------------------------------------------------------------------------


	// ------------------------------ Orientation (���⺰ ���� ����) -----------------------------
	// ĳ������ ������ �������� ������ ���⿡ ���� ������ ����
	// Orientation_Angle ���� �������� �ִϸ��̼� �׷������� ���� ��ġ�� �ڿ������� ��ȭ��Ű�鼭 �ε巯�� �ִϸ��̼� ���
	F_Orientation_Angle = Direction;
	R_Orientation_Angle = Direction - 90.0f;
	B_Orientation_Angle = Direction - 180.0f;
	L_Orientation_Angle = Direction + 90.0f;
	//------------------------------------------------------------------------


	// ------------------------------ MovementInput(Direction Angles) : �̵� ���⿡ ���� ó��-----------------------------
	// enum�� ����� ���� ĳ���Ͱ� �ٶ󺸴� ���⿡ �°� ����
	// �� ����� �ִϸ��̼� �׷��� BlendPoses(EAnimationState)�� ����� �� ���⿡ �´� �ִϸ��̼� ���
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


	// ------------------------------ Aim Offset ó�� ------------------------------
	// ĳ������ Yaw�� Pitch ���� ����Ͽ� idle ���¿��� ����� ������ �ִϸ��̼� ó��
	// Yaw���� �ִϸ��̼� �̺�Ʈ �׷������� ó�� (c++�̽� ����)
	CurPitch = UKismetMathLibrary::NormalizedDeltaRotator(MyCharacter->GetBaseAimRotation(), MyCharacter->GetActorRotation()).Pitch;
	//---------------------------------------------------------------------------


	// -------------------------- ����� �����̽� 1D Lean ------------------------------
	// �ٴ� ���⿡ �°� ���� Ʋ������ ��ɿ� ���
	CurDirectionAngle = UKismetMathLibrary::FInterpTo(CurDirectionAngle, TurnRate, GetWorld()->GetDeltaSeconds(), 10.0f);
	//-------------------------------------------------------------------------------------------------------
}

void UGnuMyAnimInstance::SetTurnRate(float CurYaw) // GnuCharacterPlayerController���� ȣ��
{
	// TurnRate : ���콺�� ī�޶� ȸ���� �� ���� ������
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