// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyPlayerController.h"
#include "Characters/GnuMyCharacter.h" // GnuMyChracter�Լ� ȣ������ ���
#include "Characters/GnuMyAnimInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


AGnuMyPlayerController::AGnuMyPlayerController()
{
	bReplicates = true;

	CurrentMoveDirection = { 0.0f, 0.0f }; // ������ ���� ������ ���
}

void AGnuMyPlayerController::BeginPlay()
{
	// �θ� Ŭ������ �ʱ�ȭ ������ �����Ͽ�, �� Ŭ�������� �����ϴ� ����̳� �����͸� �ùٸ��� �����ϱ� ���� �ʿ�
	Super::BeginPlay();

	// check() ��ũ�� : �־��� ������ ������ Ȯ���մϴ�.���� MyCharacterContext�� nullptr�̶��, �� ������ �����ϰ� ������ �ߴ�
	check(MyCharacterContext);

	// ���� ���� �÷��̾ ���� UEnhancedInputLocalPlayerSubsystem �ν��Ͻ��� ������
	// UEnhancedInputLocalPlayerSubsystem : Enhanced Input �ý����� ����� Ȱ���� �� �ִ� �ν��Ͻ��� �����ϴ� �� ���
	// GetSubsystem<>() : Ư�� ����ý����� �ν��Ͻ��� ��ȯ�ϴ� ���ø� �޼���
	// <UEnhancedInputLocalPlayerSubsystem> : � ����ý����� �������� �������� ����ϴ� �κ�. ���⼭�� Enhanced Input �ý����� ����ý����� ��û
	// GetLocalPlayer() : ���� �÷��̾ ���� ���� �÷��̾� ��ü�� ��ȯ
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		// 0�� �켱����(priority)�� �ǹ�. �켱������ ���� �Է� ���� ���ؽ�Ʈ�� ���� ��, � ���ؽ�Ʈ�� �켱 ��������� �����ϴµ� ���
		Subsystem->AddMappingContext(MyCharacterContext, 0);
	}
	
}

void AGnuMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::Move);
	EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::Rotation);
	EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::Dodge);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Sprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGnuMyPlayerController::SprintStop);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::ToggleCrouch);
	EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::ToggleCamera);
	EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::ToggleZoomIn);
	EnhancedInputComponent->BindAction(WeaponChangeAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::WeaponChange);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGnuMyPlayerController::StopJumping);

	EnhancedInputComponent->BindAction(ShotAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Fire);
	EnhancedInputComponent->BindAction(ShotAction, ETriggerEvent::Completed, this, &AGnuMyPlayerController::StopFire);
	EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::Aiming);
	EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AGnuMyPlayerController::StopAiming);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Reload);
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Interact);
}


void AGnuMyPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Controller��ü�� ���� ȸ������ ������ ���� (��� ������ �������� ã�� ��)
		// ī�޶� �ٶ󺸴� ���⿡ ���� ĳ���Ͱ� �����̵��� �ϱ� ����
		const FRotator Rotation = GetControlRotation();

		// Roll�� Pitch�� 0���� ����(ĳ���Ͱ� ������� �ʰ� ��), Yaw�� ������ ������ ȸ������ Yaw�� ����� ���ο� FRotator�� ����
		// Yaw�� ����ϸ� ���� ȸ���� ����� �� �ֱ� ������ Yaw�� ����ϴ� ��
		// ���� ������ �������� �� ȸ���� ��Ÿ��
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// FRotationMatrix�� ����Ͽ� YawRotation�� ����� ȸ�� ����� �����ϰ�, X���� �������� �� ���� ����(���� ���� ����)�� ������ ����
		// ���� ���͸� �������� ��(W,S�� �������� �� �ڷ� �����̰� �ϱ� ����)
		// FRotationMatrix : �־��� ȸ����(���⼭�� YawRotation)�� ������� 3D ���������� ȸ�� ����� �����ϴ� Ŭ���� (��ü�� ȸ���� ������ �� ���)
		// YawRotation : ĳ������ ȸ���� �����ϴ� ��. Yaw�� ���� ������ ȸ���� ��Ÿ����, �� ���� ����Ͽ� Roll(�¿� ����)�� Pitch(�յ� ����)�� 0���� �����մϴ�
		// -> ���� �� ����� ���� ���������� ȸ������ ����ϰ� �˴ϴ�.
		// GetUnitAxis() : ������ ȸ�� ��Ŀ��� Ư�� �࿡ ���� ���� ���͸� ��ȯ. ���⼭�� X�࿡ ���� ���͸� ��û
		// EAxis::X : �� �Ű������� ��ȯ�ϰ��� �ϴ� ���� ����. EAxis::X�� X��(���� ����)�� ��Ÿ����, �� �࿡ ���� ���� ���͸� ��ȯ�մϴ�.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// FRotationMatrix�� ����Ͽ� YawRotation�� ����� ȸ�� ����� �����ϰ�, Y���� �������� �� ���� ����(������ ���� ����)�� ������ ����
		// ���� ���͸� �������� ��(A,D�� �������� �� �ڷ� �����̰� �ϱ� ����)
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Y�� �Է��� ForwardDirection��, X�� �Է��� RightDirection�� ����
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); // W/S Ű
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);    // A/D Ű

		CurrentMoveDirection = InputAxisVector; // ������ ������ ���ϱ� ���� ���� ����
	}

	// FRotationMatrix�� ���� ������ ��Ŀ��� X���� ���� ������ ��Ÿ���ϴ�. ��, ĳ���Ͱ� �ٶ󺸴� �������� ���ϴ� �����Դϴ�.
	// -> �� ���ʹ� ĳ���Ͱ� ������ ���ư� ���� ������ �����մϴ�.
	// InputAxisVector.Y: InputAxisVector�� ����ڰ� �Է��� �̵� ������ ��Ÿ���� ���ͷ�, Y���� ����** ����(Forward) �Ǵ� ����(Backward)** ���� �̵��ϴ� �Է��� �ݿ��մϴ�.
	// -> W Ű�� �Ϲ������� Y���� �������� ����(����), S Ű�� �������� ����(����)���� �����˴ϴ�.
	// ������ ���� : ���⼭ Y���� ���� Positive�̸� ĳ���Ͱ� ForwardDirection(����)���� �̵��ϰ�, Negative�̸� �ݴ� �������� �̵��ϰ� �˴ϴ�.
	// ====> �׷��� ������ ��� �󿡼� X�� �޾ƿ� ���� forward�����ϰ�, �Է��� ws �� y���� �������� ������ �̵��ϴϱ� Y�� �־��ִ� ��
}
void AGnuMyPlayerController::Rotation(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		// Value�� �Է� �׼ǿ��� ���޵� ���� ��Ÿ����, Get<FVector2D>()�� �� ���� 2D ���ͷ� ��ȯ�ϴ� �� 
		// �Ϲ������� �� 2D ���ʹ� ī�޶� ȸ�� �Է��� ��Ÿ����, X���� ���� ȸ��(Yaw), Y���� ���� ȸ��(Pitch)�� �ǹ�
		// Vector2D : X�� ���� ���콺�� �¿� �̵�(����, ������), Y�� ���� ���콺�� ���� �̵�(��, �Ʒ�)
		const FVector2D RotationValue = InputActionValue.Get<FVector2D>();

		// Yaw(�¿� ȸ��) �Է� ó��
		AddYawInput(RotationValue.X);
		// Pitch(���� ȸ��) �Է� ó��
		AddPitchInput(RotationValue.Y);

		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			UGnuMyAnimInstance* AnimInstance = Cast<UGnuMyAnimInstance>(MyCharacter->GetMesh()->GetAnimInstance());
			if (AnimInstance != nullptr)
			{
				// SetTurnRate �Լ� ȣ�� (���� Yaw ���� ����)
				AnimInstance->SetTurnRate(FMath::Clamp(RotationValue.X, -1.0f, 1.0f));
			}
		}

	}
}

void AGnuMyPlayerController::Dodge(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			// DodgeSystem ȣ��
			MyCharacter->ServerMontageOnDodge(CurrentMoveDirection.Y, CurrentMoveDirection.X);

		}
	}
}

void AGnuMyPlayerController::Sprint(const FInputActionValue& InputActionValue)
{

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			if (!MyCharacter->isSprint)
			{
				// �������� ������Ʈ ���� ȣ��
				MyCharacter->ServerSprintStart();
			}
		}
	}
}

void AGnuMyPlayerController::SprintStop(const FInputActionValue& InputActionValue)
{

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			if (MyCharacter->isSprint)
			{
				// �������� ������Ʈ ���� ȣ��
				MyCharacter->ServerSprintEnd();
			}
		}
	}
}

void AGnuMyPlayerController::ToggleCrouch(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			if (MyCharacter->isCrouch)
			{
				// �������� �ɱ� ���� ȣ��
				MyCharacter->ServerCrouchEnd();
			}
			else
			{
				// �������� �ɱ� ���� ȣ��
				MyCharacter->ServerCrouchStart();
			}
		}
	}
}

void AGnuMyPlayerController::ToggleCamera(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->SetCamera(); // ī�޶� ���� ��ȯ
		}
	}
}

void AGnuMyPlayerController::ToggleZoomIn(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->SetZoomIn(); // ī�޶� ���� ����
		}
	}
}

void AGnuMyPlayerController::WeaponChange(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			UGnuMyAnimInstance* AnimInstance = Cast<UGnuMyAnimInstance>(MyCharacter->GetMesh()->GetAnimInstance());
			if (AnimInstance != nullptr)
			{
				// SetTurnRate �Լ� ȣ�� (���� Yaw ���� ����)
				AnimInstance->ServerSetAnimState_Implementation();
			}
		}
	}
}

void AGnuMyPlayerController::Jump(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->Jump(); // ĳ������ Jump �޼��� ȣ��
		}
	}
}

void AGnuMyPlayerController::StopJumping(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->StopJumping(); // ĳ������ StopJumping �޼��� ȣ��
		}
	}
}

void AGnuMyPlayerController::Aiming()
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->Aiming(); 
		}
	}
}

void AGnuMyPlayerController::StopAiming()
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->StopAiming(); 
		}
	}
}

void AGnuMyPlayerController::Fire()
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->Fire();
		}
	}
}

void AGnuMyPlayerController::StopFire()
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->StopFire();
		}
	}
}


void AGnuMyPlayerController::Reload()
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->ServerMontageOnReload();
		}
	}
}


void AGnuMyPlayerController::Interact()
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->Interact();
		}
	}
}