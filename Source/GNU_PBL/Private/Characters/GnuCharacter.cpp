// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/GnuCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Characters/GnuCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "HUD/GNUOverHeadWidget.h"
#include "PlayerController/GNUPlayerController.h"

// Sets default values
AGnuCharacter::AGnuCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	///////////////////////////
	// 캐릭터 회전
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = true;
	MovementComponent->RotationRate = FRotator(0.f, 720.f, 0.f);
	//////////////////////////
	
	// 카메라
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->SetWorldRotation(FRotator(-30.0f, 0.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	///////////////////////////

	isWaking = false;

	//
	// UI 추가 파트
	//
	/*OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);*/

	
}

// Called when the game starts or when spawned
void AGnuCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MyAnimInstance = Cast<UGnuCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSystem;
		EnhancedInputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputSystem != nullptr)
		{
			EnhancedInputSystem->AddMappingContext(MappingContext, 0);
		}
	}

	//
	// HP 업데이트
	//
	GNUPlayerController = Cast<AGNUPlayerController>(Controller);
	if (GNUPlayerController)
	{
		GNUPlayerController->SetHUDHealth(Health, MaxHealth);
		GNUPlayerController->SetHUDPlayerName(PlayerName);
	}

}

// Called every frame
void AGnuCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	isWaking = MyAnimInstance->isWalking();
}

void AGnuCharacter::Move(const FInputActionValue& value)
{
	if (GetController() != nullptr)
	{
		const FVector2D MoveVector2D = value.Get<FVector2D>();

		const FRotator ControllerRotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0.0f, ControllerRotation.Yaw, 0.0f);

		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, MoveVector2D.X);

		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Right, MoveVector2D.Y);

		UpdateAnimInstance(MoveVector2D);
	}
}

void AGnuCharacter::UpdateAnimInstance(const FVector2D& MoveVector2D)
{
	if (MyAnimInstance)
	{
		// Set all movement direction variables to false initially
		MyAnimInstance->SetIsStraightMove(false);
		MyAnimInstance->SetIsBackMove(false);
		MyAnimInstance->SetIsLeftMove(false);
		MyAnimInstance->SetIsRightMove(false);

		// Determine which direction key is being pressed
		if (MoveVector2D.X > 0.0f)  // Moving forward
		{
			MyAnimInstance->SetIsStraightMove(true);
		}
		else if (MoveVector2D.X < 0.0f)  // Moving backward
		{
			MyAnimInstance->SetIsBackMove(true);
		}

		if (MoveVector2D.Y < 0.0f)  // Moving left
		{
			MyAnimInstance->SetIsLeftMove(true);
		}
		else if (MoveVector2D.Y > 0.0f)  // Moving right
		{
			MyAnimInstance->SetIsRightMove(true);
		}
	}
}


void AGnuCharacter::Rotation(const FInputActionValue& value)
{
	if (GetController() != nullptr)
	{
		FVector2D RotationValue;
		RotationValue = value.Get<FVector2D >();
		AddControllerYawInput(RotationValue.X);
		AddControllerPitchInput(RotationValue.Y);
	}
}

// Called to bind functionality to input
void AGnuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGnuCharacter::Move);
		EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Triggered, this, &AGnuCharacter::Rotation);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	}
}



void AGnuCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGnuCharacter, Health);
	DOREPLIFETIME(AGnuCharacter, PlayerName);
}

void AGnuCharacter::OnRep_PlayerName()
{
	
}

/// 
/// HP 바
/// 
void AGnuCharacter::OnRep_Health()
{

}



