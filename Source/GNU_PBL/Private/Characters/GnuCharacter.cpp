// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/GnuCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Characters/GnuCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/Gun.h"
#include "Weapons/CrossHair.h"
#include "Weapons/WeaponSwitch.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AGnuCharacter::AGnuCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	///////////////////////////
	// 캐릭터 회전
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
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

	static ConstructorHelpers::FClassFinder<UUserWidget> CrossHairFinder(TEXT("/Game/GNU/weapon/UI_CrossHair.UI_CrossHair_C"));
	if (CrossHairFinder.Succeeded())
	{
		CrossHairWidgetClass = CrossHairFinder.Class;
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
		}
	}

	if (CrossHairWidgetClass) // 크로스헤어 UI 유효성 검사
	{
		// Create and add the CrossHair widget to the viewport
		pCrossHair = CreateWidget<UCrossHair>(GetWorld(), CrossHairWidgetClass);
		if (IsValid(pCrossHair))
		{
			pCrossHair->AddToViewport(); // 뷰포트 추가
		}
	}

	// Bind aim rate to CrossHair
	if (AGnuCharacter* GnuCharacter = Cast<AGnuCharacter>(GetController() ? GetController()->GetPawn() : nullptr))
	{
		if (GnuCharacter)
		{
			pCrossHair->BindUserAimRate(GnuCharacter);
		}
	}

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
	Gun->UpdateAmmoDisplay();
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

void AGnuCharacter::Aiming()
{
	if (GetController() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Aiming..."));
		if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(0);
		}
		const FRotator ControllerRotation = GetController()->GetControlRotation();
		float Pitch = ControllerRotation.Pitch;
		if (Pitch > 90) {
			Pitch -= 360;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Pitch : %f"), Pitch);
		MyAnimInstance->SetAimPitch(Pitch);
	}
}

void AGnuCharacter::StopAiming()
{
	if (GetController() != nullptr)
	{
		if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(1);
		}
	}
}

void AGnuCharacter::StartFire()
{
	if (GetController() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("fire!"));
		if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(2); // 새로운 AimRate로 크로스헤어 업데이트

		}
		Gun->PullTrigger();
	}
}

void AGnuCharacter::StopFire()
{
	if (GetController() != nullptr)
	{
		if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(1);
		}
		Gun->ReleaseTrigger();
	}
}


void AGnuCharacter::Reroad()
{
	if (GetController() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reroad!"));
		if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(3); // 새로운 AimRate로 크로스헤어 업데이트
		}
		Gun->Reload();
	}
}

void AGnuCharacter::Interact()
{
	if (GetController() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact"));
		AController* PlayerController = GetController();
		if (PlayerController == nullptr)
		{
			return;
		}

		FVector Location;
		FRotator Rotation;
		PlayerController->GetPlayerViewPoint(Location, Rotation);

		FVector End = Location + Rotation.Vector() * 1000;
		// TODO: LineTrace 

		FHitResult Hit;
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);
		

		if (bSuccess)
		{
			// 히트된 액터가 존재하는지 확인
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				// 액터에 특정 태그가 있는지 확인 (예: "WeaponSwitch")
				if (HitActor->ActorHasTag(FName("WeaponSwitch")))
				{
					AWeaponSwitch* NewGun = Cast<AWeaponSwitch>(HitActor);
					if (NewGun)
					{
						UE_LOG(LogTemp, Warning, TEXT("Switching Weapon..."));
						TSubclassOf<AGun> NewGunClass = NewGun->Switching();
						SwitchWeapon(NewGunClass);
					}					
				}
			}
		}
	}
}

void AGnuCharacter::SwitchWeapon(TSubclassOf<AGun> NewGunClass)
{
	if (NewGunClass) 
	{
		GunClass = NewGunClass;
		
		if (Gun)
		{
			Gun->RemoveAmmoDisplay();
			Gun->Destroy();
			Gun = nullptr;
		}
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);

		if (Gun)
		{
			Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
			Gun->SetOwner(this);
			Gun->UpdateAmmoDisplay();
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
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AGnuCharacter::StartFire);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AGnuCharacter::StopFire);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Triggered, this, &AGnuCharacter::Aiming);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AGnuCharacter::StopAiming);
		EnhancedInputComponent->BindAction(ReroadAction, ETriggerEvent::Started, this, &AGnuCharacter::Reroad);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AGnuCharacter::Interact); //상호작용 
	}
}

