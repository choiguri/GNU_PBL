// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuCharacterPlayerController.h"
#include "Characters/GnuMyCharacter.h" // GnuMyChracter함수 호출위해 사용
#include "Characters/GnuMyCharacterAnimInstance.h" // GnuMyChracter함수 호출위해 사용
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


AGnuCharacterPlayerController::AGnuCharacterPlayerController()
{
	bReplicates = true;

	CurrentMoveDirection = { 0.0f, 0.0f }; // 구르기 방향 결정에 사용
}

void AGnuCharacterPlayerController::BeginPlay()
{
	// 부모 클래스의 초기화 로직을 실행하여, 이 클래스에서 제공하는 기능이나 데이터를 올바르게 설정하기 위해 필요
	Super::BeginPlay();

	// check() 매크로 : 주어진 조건이 참인지 확인합니다.만약 MyCharacterContext가 nullptr이라면, 이 조건은 실패하고 게임이 중단
	check(MyCharacterContext);
	
	// 현재 로컬 플레이어에 대한 UEnhancedInputLocalPlayerSubsystem 인스턴스를 가져옴
	// UEnhancedInputLocalPlayerSubsystem : Enhanced Input 시스템의 기능을 활용할 수 있는 인스턴스를 저장하는 데 사용
	// GetSubsystem<>() : 특정 서브시스템의 인스턴스를 반환하는 템플릿 메서드
	// <UEnhancedInputLocalPlayerSubsystem> : 어떤 서브시스템을 가져오고 싶은지를 명시하는 부분. 여기서는 Enhanced Input 시스템의 서브시스템을 요청
	// GetLocalPlayer() : 현재 플레이어에 대한 로컬 플레이어 객체를 반환
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		// 0은 우선순위(priority)를 의미. 우선순위는 여러 입력 매핑 컨텍스트가 있을 때, 어떤 컨텍스트가 우선 적용될지를 결정하는데 사용
		Subsystem->AddMappingContext(MyCharacterContext, 0);
	}

	//bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Default;

	//FInputModeGameAndUI InputModeData;
	//InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	//InputModeData.SetHideCursorDuringCapture(false);
	//SetInputMode(InputModeData);
}

void AGnuCharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::Move);
	EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::Rotation);
	EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::Dodge);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::ToggleSprint);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::ToggleCrouch);
	EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::ToggleCamera);
	EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::ToggleZoomIn);
	EnhancedInputComponent->BindAction(WeaponChangeAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::WeaponChange);
	EnhancedInputComponent->BindAction(PistolChangeAction, ETriggerEvent::Triggered, this, &AGnuCharacterPlayerController::PistolChange);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGnuCharacterPlayerController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGnuCharacterPlayerController::StopJumping);
}


void AGnuCharacterPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Controller객체의 현재 회전값을 가져와 저장 (어느 방향이 앞쪽인지 찾는 것)
		// 카메라가 바라보는 방향에 따라 캐릭터가 움직이도록 하기 위함
		const FRotator Rotation = GetControlRotation();

		// Roll과 Pitch를 0으로 설정(캐릭터가 기울지지 않게 함), Yaw는 이전에 가져온 회전값의 Yaw를 사용해 새로운 FRotator를 만듦
		// Yaw를 사용하면 수평 회점나 고려할 수 있기 때문에 Yaw를 사용하는 것
		// 수평 방향을 기준으로 한 회전을 나타냄
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// FRotationMatrix를 사용하여 YawRotation에 기반한 회전 행렬을 생성하고, X축을 기준으로 한 단위 벡터(앞쪽 방향 벡터)를 가져와 저장
		// 전방 벡터를 가져오는 것(W,S를 눌렀을때 앞 뒤로 움직이게 하기 위함)
		// FRotationMatrix : 주어진 회전값(여기서는 YawRotation)을 기반으로 3D 공간에서의 회전 행렬을 생성하는 클래스 (물체의 회전을 적용할 때 사용)
		// YawRotation : 캐릭터의 회전을 정의하는 값. Yaw는 수평 방향의 회전을 나타내며, 이 값만 사용하여 Roll(좌우 기울기)와 Pitch(앞뒤 기울기)는 0으로 설정합니다
		// -> 따라서 이 행렬은 수평 방향으로의 회전만을 고려하게 됩니다.
		// GetUnitAxis() : 생성된 회전 행렬에서 특정 축에 대한 단위 벡터를 반환. 여기서는 X축에 대한 벡터를 요청
		// EAxis::X : 이 매개변수는 반환하고자 하는 축을 지정. EAxis::X는 X축(앞쪽 방향)을 나타내며, 이 축에 대한 단위 벡터를 반환합니다.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// FRotationMatrix를 사용하여 YawRotation에 기반한 회전 행렬을 생성하고, Y축을 기준으로 한 단위 벡터(오른쪽 방향 벡터)를 가져와 저장
		// 우측 벡터를 가져오는 것(A,D를 눌렀을때 앞 뒤로 움직이게 하기 위함)
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Y축 입력은 ForwardDirection에, X축 입력은 RightDirection에 매핑
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y); // W/S 키
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);    // A/D 키

		CurrentMoveDirection = InputAxisVector; // 구르기 방향을 정하기 위해 방향 저장
	}

	// FRotationMatrix를 통해 생성된 행렬에서 X축은 앞쪽 방향을 나타냅니다. 즉, 캐릭터가 바라보는 방향으로 향하는 벡터입니다.
	// -> 이 벡터는 캐릭터가 앞으로 나아갈 때의 방향을 정의합니다.
	// InputAxisVector.Y: InputAxisVector는 사용자가 입력한 이동 방향을 나타내는 벡터로, Y축의 값은** 앞쪽(Forward) 또는 뒤쪽(Backward)** 으로 이동하는 입력을 반영합니다.
	// -> W 키는 일반적으로 Y축의 긍정적인 방향(앞쪽), S 키는 부정적인 방향(뒤쪽)으로 설정됩니다.
	// 매핑의 원리 : 여기서 Y축의 값이 Positive이면 캐릭터가 ForwardDirection(앞쪽)으로 이동하고, Negative이면 반대 방향으로 이동하게 됩니다.
	// ====> 그렇기 때문에 행렬 상에서 X를 받아온 것을 forward저장하고, 입력은 ws 즉 y축을 기점으로 앞으로 이동하니까 Y에 넣어주는 것
}
void AGnuCharacterPlayerController::Rotation(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		// Value는 입력 액션에서 전달된 값을 나타내며, Get<FVector2D>()는 이 값을 2D 벡터로 변환하는 것 
		// 일반적으로 이 2D 벡터는 카메라 회전 입력을 나타내며, X축은 수평 회전(Yaw), Y축은 수직 회전(Pitch)을 의미
		// Vector2D : X축 값은 마우스의 좌우 이동(왼쪽, 오른쪽), Y축 값은 마우스의 상하 이동(위, 아래)
		const FVector2D RotationValue = InputActionValue.Get<FVector2D>();
		
		// Yaw(좌우 회전) 입력 처리
		AddYawInput(RotationValue.X);
		// Pitch(상하 회전) 입력 처리
		AddPitchInput(RotationValue.Y);

		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			UGnuMyCharacterAnimInstance* AnimInstance = Cast<UGnuMyCharacterAnimInstance>(MyCharacter->GetMesh()->GetAnimInstance());
			if (AnimInstance != nullptr)
			{
				// SetTurnRate 함수 호출 (현재 Yaw 값을 전달)
				AnimInstance->SetTurnRate(RotationValue.X);
			}
		}

	}
}

void AGnuCharacterPlayerController::Dodge(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			// DodgeSystem 호출
			MyCharacter->ServerMontageOnDodge(CurrentMoveDirection.Y, CurrentMoveDirection.X);
			// 가만히 있으면 구르기 막기 위함
			CurrentMoveDirection = { 0.0f, 0.0f };
		}
	}
}

void AGnuCharacterPlayerController::ToggleSprint(const FInputActionValue& InputActionValue)
{

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			if (MyCharacter->isSprint)
			{
				// 서버에서 스프린트 종료 호출
				MyCharacter->ServerSprintEnd();
			}
			else
			{
				// 서버에서 스프린트 시작 호출
				MyCharacter->ServerSprintStart();
			}
		}
	}
}

void AGnuCharacterPlayerController::ToggleCrouch(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			if (MyCharacter->isCrouch)
			{
				// 서버에서 스프린트 종료 호출
				MyCharacter->ServerCrouchEnd();
			}
			else
			{
				// 서버에서 스프린트 시작 호출
				MyCharacter->ServerCrouchStart();
			}
		}
	}
}

void AGnuCharacterPlayerController::ToggleCamera(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->SetCamera(); // Crouch 설정
		}
	}
}

void AGnuCharacterPlayerController::ToggleZoomIn(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->SetZoomIn(); // Crouch 설정
		}
	}
}

void AGnuCharacterPlayerController::PistolChange(const FInputActionValue& InputActionValue)
{

}

void AGnuCharacterPlayerController::WeaponChange(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			UGnuMyCharacterAnimInstance* AnimInstance = Cast<UGnuMyCharacterAnimInstance>(MyCharacter->GetMesh()->GetAnimInstance());
			if (AnimInstance != nullptr)
			{
				// SetTurnRate 함수 호출 (현재 Yaw 값을 전달)
				AnimInstance->ServerSetWeapon();
			}
		}
	}
}

void AGnuCharacterPlayerController::Jump(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->Jump(); // 캐릭터의 Jump 메서드 호출
		}
	}
}

void AGnuCharacterPlayerController::StopJumping(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->StopJumping(); // 캐릭터의 StopJumping 메서드 호출
		}
	}
}
