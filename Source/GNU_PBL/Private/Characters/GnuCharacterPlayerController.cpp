// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuCharacterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AGnuCharacterPlayerController::AGnuCharacterPlayerController()
{
	bReplicates = true;
}

void AGnuCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(MyCharacterContext);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
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
}

void AGnuCharacterPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Y축 입력은 ForwardDirection에, X축 입력은 RightDirection에 매핑
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.X); // W/S 키
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.Y);    // A/D 키
	}
}
void AGnuCharacterPlayerController::Rotation(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector2D RotationValue;
		RotationValue = InputActionValue.Get<FVector2D >();
		AddYawInput(RotationValue.X);
		AddPitchInput(RotationValue.Y);
	}
}
