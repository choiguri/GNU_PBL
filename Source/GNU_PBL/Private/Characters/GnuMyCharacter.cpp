// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCharacterMovement()
#include "Components/CapsuleComponent.h" // SetCapsuleHalfHeight()
#include "GameFramework/SpringArmComponent.h" // SpringArm
#include "Camera/CameraComponent.h" // TPP, FPP Camera
#include "Characters/GnuMyAnimInstance.h" // MultiCastMontage_Dodge_Implementation �Լ��� ��� �Լ���
#include "Animation/AnimMontage.h" // class UAnimMontage
#include "Net/UnrealNetwork.h" // ~_Implementation(), ~_Validate(), GetLifetimeReplicatedProps()
#include "Weapons/Gun.h"
#include "Weapons/CrossHair.h"
#include "Weapons/WeaponSwitch.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AGnuMyCharacter::AGnuMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ------------------------------- ĳ���� ȸ�� ------------------------------
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 400.f, 0.f);
	// ---------------------------------------------------------------

	// ------------------------------- Camera �ʱ�ȭ ------------------------------
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); // RootComponent�� ����
	CameraBoom->TargetArmLength = 350.0f; // ���ϴ� ���̷� ����
	CameraBoom->SocketOffset = FVector(0.0f, 80.0f, 40.0f);
	CameraBoom->bUsePawnControlRotation = true; // ī�޶� ȸ�� ���

	TPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPPCamera"));
	TPPCamera->SetupAttachment(CameraBoom); // ī�޶�տ� ����
	TPPCamera->bUsePawnControlRotation = false;

	FPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPPCamera"));
	FPPCamera->SetupAttachment(GetMesh(), TEXT("Head")); // Head ���Ͽ� ����
	FPPCamera->bUsePawnControlRotation = true;

	isFPPCamera = false;
	// ---------------------------------------------------------------

	// ---------------------- Movement ���� �ʱ�ȭ -----------------------
	DefaultSpeed = 300.0f;
	SprintSpeed = 600.0f;
	MovementComponent->MaxWalkSpeed = DefaultSpeed;
	bReplicates = true;
	isSprint = false;
	isDodge = false;
	isCrouch = false;
	isZoomIn = false;
	DodgeMontage = nullptr;
	// ---------------------------------------------------------------

	// -------------------- CrossHair create -----------------
	static ConstructorHelpers::FClassFinder<UUserWidget> CrossHairFinder(TEXT("/Game/GNU/weapon/UI_CrossHair.UI_CrossHair_C"));
	if (CrossHairFinder.Succeeded())
	{
		CrossHairWidgetClass = CrossHairFinder.Class;
	}
}

void AGnuMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ---------------- ZoomIn �ε巴�� �����Ǵ� Ÿ�Ӷ��� �ʱ�ȭ ----------------
	// FOnTimelineFloat : Ÿ�Ӷ��ο��� �÷�Ʈ(float) ���� ��ȯ�ϴ� ��������Ʈ Ÿ��
	FOnTimelineFloat ZoomInProgressUpdate;
	// BindUFunction : ZoomInUpdate�̶�� �̸��� ���� �Լ��� ���ε�
	// Ÿ�Ӷ����� ����� ������ ZoomInUpdate �Լ��� ȣ��
	ZoomInProgressUpdate.BindUFunction(this, FName("ZoomInUpdate"));

	// FOnTimelineEvent : Ÿ�Ӷ����� ������ �� ȣ��Ǵ� �̺�Ʈ ��������Ʈ
	FOnTimelineEvent ZoomInFinishedEvent;
	// ZoomInFinished�̶�� �̸��� ���� �Լ��� ���ε�
	ZoomInFinishedEvent.BindUFunction(this, FName("ZoomInFinished"));

	// AddInterpFloat : �÷�Ʈ �(Curve)�� �߰�, �ش� ��� ���� ���� ��ȭ��Ű�鼭 ��������Ʈ(ZoomInProgressUpdate)�� ȣ��
	ZoomInTimeline.AddInterpFloat(ZoomInCurve, ZoomInProgressUpdate);
	// SetTimelineFinishedFunc : Ÿ�Ӷ����� ������ �� ȣ���� ��������Ʈ(�̺�Ʈ)�� ����
	ZoomInTimeline.SetTimelineFinishedFunc(ZoomInFinishedEvent);
	// SetTimelineLength : Ÿ�Ӷ��� ���� ����
	ZoomInTimeline.SetTimelineLength(0.3f);
	// -------------------------------------------------------------------------------

	
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
	if (AGnuMyCharacter* GnuMyCharacter = Cast<AGnuMyCharacter>(GetController() ? GetController()->GetPawn() : nullptr))
	{
		if (GnuMyCharacter)
		{
			pCrossHair->BindUserAimRate(GnuMyCharacter);
		}
	}

	// Gun Actor Create
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
	Gun->UpdateAmmoDisplay();
}

void AGnuMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ZoomInTimeline.TickTimeline(DeltaTime);
}

void AGnuMyCharacter::SetCamera()
{
	isFPPCamera = !isFPPCamera; // ���� ���¸� ������Ŵ

	if (isFPPCamera)
	{
		// 1��Ī ī�޶� Ȱ��ȭ
		if (FPPCamera && TPPCamera) // Null üũ
		{
			TPPCamera->SetActive(false);
			FPPCamera->SetActive(true);
		}
	}
	else
	{
		// 3��Ī ī�޶� Ȱ��ȭ
		if (FPPCamera && TPPCamera) // Null üũ
		{
			TPPCamera->SetActive(true);
			FPPCamera->SetActive(false);
		}
	}
}


// ------------------------ ī�޶� ���� �� �ƿ� -----------------------
void AGnuMyCharacter::ZoomInUpdate(float Alpha)
{
	float NewArmLength = 0.0f;

	// Lerp : �� �� ���̸� ���������� �����ϴ� �Լ�
	// Alpha : 0 ~ 1���� ������ ������ �󸶳� ����Ǿ����� ��Ÿ��
	NewArmLength = FMath::Lerp(350.0f, 150.0f, Alpha);
	FVector StartOffset(0.0f, 80.0f, 40.0f);
	FVector EndOffset(0.0f, 80.0f, 60.0f);

	// CameraBoom�� StartOffset�� ������Ʈ
	FVector NewSocketOffset = FMath::Lerp(StartOffset, EndOffset, Alpha);
	CameraBoom->SocketOffset = NewSocketOffset;
	// CameraBoom�� TargetArmLength�� ������Ʈ
	CameraBoom->TargetArmLength = NewArmLength;
}

void AGnuMyCharacter::ZoomInFinished()
{

}

void AGnuMyCharacter::SetZoomIn()
{
	isZoomIn = !isZoomIn; // ���� ���¸� ������Ŵ

	if (isZoomIn)
	{
		if (ZoomInCurve) // Null üũ
		{
			ZoomInTimeline.PlayFromStart();
		}
	}
	else
	{
		if (ZoomInCurve) // Null üũ
		{
			ZoomInTimeline.ReverseFromEnd();
		}
	}
}
// -----------------------------------------------------------------


void AGnuMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ������ ��Ʈ��ũ���� ������ �� �ֵ��� ����
	DOREPLIFETIME(AGnuMyCharacter, isSprint);
	DOREPLIFETIME(AGnuMyCharacter, isCrouch);
}

// ----------------------------------------- Sprint Replicate-----------------------------------
void AGnuMyCharacter::ServerSprintStart_Implementation() // Ŭ���̾�Ʈ���� ������Ʈ ���� ��û�� ������ �������� ServerSprintStart_Implementation�� ����
{
	UpdateSprintState(true); //  �� �Լ��� ���������� ����
}

bool AGnuMyCharacter::ServerSprintStart_Validate()
{
	return true; // �ʿ��� ��ȿ�� �˻� ������ ���⿡ �߰�
}

void AGnuMyCharacter::ServerSprintEnd_Implementation()
{
	UpdateSprintState(false);
}

bool AGnuMyCharacter::ServerSprintEnd_Validate()
{
	return true; // �ʿ��� ��ȿ�� �˻� ������ ���⿡ �߰�
}

void AGnuMyCharacter::ClientSprintStart_Implementation() // Ŭ���̾�Ʈ�� ��� �ݿ��ϴ� ��������� ���� - Ŭ���̾�Ʈ �ϰ����� ������
{
	// �ּ��� Ǯ�� Ŭ���̾�Ʈ���� ������Ʈ ���¸� ������ �ݿ��� �� ������, �� ����� ������ ����� ��ٸ��� �ʱ� ������ ������ Ŭ���̾�Ʈ�� �ϰ����� ������ �� ����
	// UpdateSprintState(true);
}

void AGnuMyCharacter::ClientSprintEnd_Implementation()
{

	// UpdateSprintState(false);
}

void AGnuMyCharacter::OnRep_IsSprinting() 	// Ŭ���̾�Ʈ�� ����ȭ �Ǵ� �Լ� -> �������� isSprint ���� ����� �� Ŭ���̾�Ʈ���� �� ��ȭ�� �����ϰ� ����Ǵ� �Լ� (��, ������ �����ϴ� ������Ʈ ���¸� Ŭ���̾�Ʈ�� ����ȭ �ϴ� ���)
{
	// Client�� Implementation �Լ������� �ᵵ ������ ����� �۵��ȴ�.
	// -> �ٸ� Clint �Լ� ���ο��� ����ϸ� �� �� ���� �ӵ��� ���� �ݸ�, OnRep �Լ����� ���� ������ Ŭ���̾�Ʈ���� �ϰ����� ������ �� �ִ�.
	UpdateSprintState(isSprint); // ������Ʈ ���°� ����� ���� Ŭ���̾�Ʈ���� �ݿ�
}

void AGnuMyCharacter::UpdateSprintState(bool bIsSprinting)
{
	isSprint = bIsSprinting;

	if (isSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed; // �⺻ �ӵ� ����
	}
}

// ------------------------------------------------------------------------------------


// ----------------------------Crouch Replicate----------------------------------
void AGnuMyCharacter::ServerCrouchStart_Implementation()
{
	isCrouch = true;
}

bool AGnuMyCharacter::ServerCrouchStart_Validate()
{
	return true; // �ʿ��� ��ȿ�� �˻� ������ ���⿡ �߰�
}

void AGnuMyCharacter::ServerCrouchEnd_Implementation()
{
	isCrouch = false;
}

bool AGnuMyCharacter::ServerCrouchEnd_Validate()
{
	return true; // �ʿ��� ��ȿ�� �˻� ������ ���⿡ �߰�
}

void AGnuMyCharacter::ClientCrouchStart_Implementation()
{
}

void AGnuMyCharacter::ClientCrouchEnd_Implementation()
{
}

void AGnuMyCharacter::OnRep_IsCrouching()
{
	// �ɱ� ���¿� ���� ���� ����
	if (isCrouch)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}
// -------------------------------------------------------------------------


// --------------------------------- Dodge Replicate ------------------------------------
void AGnuMyCharacter::ServerMontageOnDodge_Implementation(float Forward, float Right) // �������� ������ �ִϸ��̼��� ó���ϴ� �Լ�
{
	if (isDodge)
	{
		// ������ �߿� �ٽ� ȣ���ϸ� ����
		return;
	}

	// ��Ÿ�� ����
	SetDodgeMontage(Forward, Right);

	// ��Ÿ�ְ� ��ȿ�� ��� ���
	if (DodgeMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		PlayAnimMontage(DodgeMontage);

		// ��Ƽĳ��Ʈ ȣ��
		MultiCastMontage_Dodge(Forward, Right);
	}
}

bool AGnuMyCharacter::ServerMontageOnDodge_Validate(float Forward, float Right)
{
	return true; // ���� ������ �ʿ��ϸ� �߰�
}

void AGnuMyCharacter::MultiCastMontage_Dodge_Implementation(float Forward, float Right) // ��� Ŭ���̾�Ʈ���� ������ ������ �ִϸ��̼��� ����ϵ��� �ϴ� ��Ƽĳ��Ʈ �Լ�. Ŭ���̾�Ʈ�� ���� ���� ����ȭ�� ���� ���
{
	if (isDodge)
	{
		// ������ �߿� �ٽ� ȣ���ϸ� ����
		return;
	}

	// ��Ÿ�� ����
	SetDodgeMontage(Forward, Right);

	// ��Ÿ�ְ� ��ȿ�� ��� ���
	if (DodgeMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		PlayAnimMontage(DodgeMontage);
	}
}

void AGnuMyCharacter::SetDodgeMontage(float Forward, float Right)
{
	DodgeMontage = nullptr;

	if (Forward == 0)
	{
		// Forward�� 0�� ���, Right�� Ȯ��
		if (Right != 0)
		{
			// Right�� 0���� ũ�� DiveRoll_R_Montage, �׷��� ������ DiveRoll_L_Montage ����
			DodgeMontage = (Right > 0) ? DiveRoll_R_Montage : DiveRoll_L_Montage;
		}
	}
	else
	{
		// Forward�� 0���� ũ�� DiveRoll_F_Montage, �׷��� ������ DiveRoll_B_Montage ����
		DodgeMontage = (Forward > 0) ? DiveRoll_F_Montage : DiveRoll_B_Montage;
	}
}
// -------------------------------------------------------------------------

void AGnuMyCharacter::Aiming()
{
	if (GetController() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Aiming..."));
		if (CameraBoom->TargetArmLength > 250.0f)
		{
			CameraBoom->TargetArmLength -= 10;
		}
		if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(0.5);
		}
	}
}

void AGnuMyCharacter::StopAiming()
{
	if (GetController() != nullptr)
	{
		CameraBoom->TargetArmLength = 350.0f;
		if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(1);
		}
	}
}

void AGnuMyCharacter::StartFire()
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

void AGnuMyCharacter::StopFire()
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


void AGnuMyCharacter::Reroad()
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

void AGnuMyCharacter::Interact()
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

void AGnuMyCharacter::SwitchWeapon(TSubclassOf<AGun> NewGunClass)
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

/* ----------- ���� - Ŭ���̾�Ʈ ����ȭ ���� -------------------
< ���ø����̼� ��� >
������Ʈ: isSprint ������ �������� �����ϰ�, �� ���� ������ OnRep_IsSprinting�� ���� Ŭ���̾�Ʈ�� �˸��� ����. �� Ŭ���̾�Ʈ�� �����κ��� ���������� ���¸� ����ȭ����
������ : isDodge ������ �ܼ� ���� ǥ�ÿ��̰�, ������ �ִϸ��̼��� ������ �� �� ó���ϰ� ��� Ŭ���̾�Ʈ���� ���� ����ǰԲ� ��Ƽĳ��Ʈ�� ���ĵ�

< �Լ� ȣ�� ��� >
������Ʈ: Ŭ���̾�Ʈ�� ������ ��û�� ������, ������ ���� Ŭ���̾�Ʈ���� �ٽ� ���¸� �����ϴ� ����� ��� ���
������ : Ŭ���̾�Ʈ�� ������ ��û�ϸ�, ������ ��� Ŭ���̾�Ʈ�� ���ÿ� ����� �����ϴ� �� ���� ���� ���
------------------------------------------------------------- */