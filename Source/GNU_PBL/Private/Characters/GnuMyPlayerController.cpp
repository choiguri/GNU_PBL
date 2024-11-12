// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/GnuMyPlayerController.h"
#include "Characters/GnuMyCharacter.h" // GnuMyChracter�Լ� ȣ������ ���
#include "Characters/GnuMyAnimInstance.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


// 추가사항
#include "HUD/GNUHUD.h"
#include "HUD/GNUCharacterOverlay.h"
#include "HUD/GNUOverHeadWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "HUD/GNUReturnToMainMenu.h"


AGnuMyPlayerController::AGnuMyPlayerController()
{
	bReplicates = true;

	CurrentMoveDirection = { 0.0f, 0.0f }; // 구르기 방향 결정에 사용
	// 생성자 정의
}


void AGnuMyPlayerController::BeginPlay()
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
}

//
// 추가사항 여러 개
//
void AGnuMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);

}

void AGnuMyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AGnuMyPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	bool bHUDValid = GNUHUD &&
		GNUHUD->CharacterOverlay &&
		GNUHUD->CharacterOverlay->HealthBar &&
		GNUHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		GNUHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		GNUHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AGnuMyPlayerController::SetHUDStamina(float Stamina, float MaxStamina)
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	bool bHUDValid = GNUHUD &&
		GNUHUD->CharacterOverlay &&
		GNUHUD->CharacterOverlay->StaminaBar &&
		GNUHUD->CharacterOverlay->StaminaText;

	if (bHUDValid)
	{
		const float HealthPercent = Stamina / MaxStamina;
		GNUHUD->CharacterOverlay->StaminaBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Stamina), FMath::CeilToInt(MaxStamina));
		GNUHUD->CharacterOverlay->StaminaText->SetText(FText::FromString(HealthText));
	}
}

void AGnuMyPlayerController::SetHUDCombatTime(float CombatTime)
{
	GNUHUD = GNUHUD == nullptr ? Cast<AGNUHUD>(GetHUD()) : GNUHUD;

	bool bHUDValid = GNUHUD &&
		GNUHUD->CharacterOverlay &&
		GNUHUD->CharacterOverlay->CombatTimeText;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CombatTime / 60.f);
		int32 Seconds = CombatTime - Minutes * 60;

		FString TimeText = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
		GNUHUD->CharacterOverlay->CombatTimeText->SetText(FText::FromString(TimeText));
	}
}


void AGnuMyPlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(TotalTime - GetServertime());

	if (CountdownInt != SecondsLeft)
	{
		SetHUDCombatTime(TotalTime - GetServertime());

	}

	CountdownInt = SecondsLeft;
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

	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Interact);
	EnhancedInputComponent->BindAction(ShotAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Fire);
	EnhancedInputComponent->BindAction(ShotAction, ETriggerEvent::Completed, this, &AGnuMyPlayerController::StopFire);
	EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::Aiming);
	EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AGnuMyPlayerController::StopAiming);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::Reload);
	EnhancedInputComponent->BindAction(ArrowSkillAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::ArrowSkill);
	EnhancedInputComponent->BindAction(HealSkillAction, ETriggerEvent::Triggered, this, &AGnuMyPlayerController::HealSkill);

	// 추가사항
	EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &AGnuMyPlayerController::ShowReturnToMainMenu);
}

// 추가사항
void AGnuMyPlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UGNUReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}

	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}

}

void AGnuMyPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;

	}
}





// 서버와 클라이언트 사이의 RoundTripTime 계산 하기 위한 함수들
// 클라이언트가 요청하고 서버에 도착하는 시간, 서버에서 클라이언트로 응답하는 시간
// 두 개를 합쳐서 RTT, 하지만 두 개가 같지는 않아서 RTT / 2 가 각각의 시간이라고 장담하지는 못한다.
float AGnuMyPlayerController::GetServertime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AGnuMyPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AGnuMyPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

// 여기까지

void AGnuMyPlayerController::Move(const struct FInputActionValue& InputActionValue)
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
void AGnuMyPlayerController::Rotation(const FInputActionValue& InputActionValue)
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
			UGnuMyAnimInstance* AnimInstance = Cast<UGnuMyAnimInstance>(MyCharacter->GetMesh()->GetAnimInstance());
			if (AnimInstance != nullptr)
			{
				// SetTurnRate 함수 호출 (현재 Yaw 값을 전달)
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
			// DodgeSystem 호출
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
				// 서버에서 스프린트 시작 호출
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
				// 서버에서 스프린트 종료 호출
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
				// 서버에서 앉기 종료 호출
				MyCharacter->ServerCrouchEnd();
			}
			else
			{
				// 서버에서 앉기 시작 호출
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
			MyCharacter->SetCamera(); // 카메라 시점 전환
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
			MyCharacter->SetZoomIn(); // 카메라 줌인 설정
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
			MyCharacter->Jump(); // 캐릭터의 Jump 메서드 호출
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
			MyCharacter->StopJumping(); // 캐릭터의 StopJumping 메서드 호출
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

void AGnuMyPlayerController::ArrowSkill(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->SpawnArrow(); // 캐릭터의 StopJumping 메서드 호출
		}
	}
}

void AGnuMyPlayerController::HealSkill(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(ControlledPawn);
		if (MyCharacter)
		{
			MyCharacter->SpawnHeal(); // 캐릭터의 StopJumping 메서드 호출
		}
	}
}