// Fill out your copyright notice in the Description page of Project Settings.

//GnuCharacter.cpp

#include "Characters/GnuCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Characters/GnuCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"  // 디버깅 메시지 출력을 위한 헤더

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
}

void AGnuCharacter::PlayRollMontage()
{
	UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance)
	{
		instance->Montage_Play(RollMontage);
	}
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
		
			UKismetSystemLibrary::PrintString(this, TEXT("입력 매핑 컨텍스트가 추가되었습니다!"));

		}
	}
}

void AGnuCharacter::Roll()
{
	// 디버깅용 메시지 출력
	UKismetSystemLibrary::PrintString(this, TEXT("Ctrl 키가 눌렸습니다!"));

	// 위치 변경

	// 구르기 애니메이션 실행
	PlayRollMontage();
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
		
		if (RollAction)
		{
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &AGnuCharacter::Roll);
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &AGnuCharacter::Roll);
			
			UKismetSystemLibrary::PrintString(this, TEXT("RollAction이 바인딩되었습니다!"));
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("RollAction이 설정되지 않았습니다."));
		}
	}
}


