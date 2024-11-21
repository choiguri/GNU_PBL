// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCharacterMovement()
#include "Components/CapsuleComponent.h" // SetCapsuleHalfHeight()
#include "GameFramework/SpringArmComponent.h" // SpringArm
#include "Camera/CameraComponent.h" // TPP, FPP Camera
#include "Characters/GnuMyAnimInstance.h" // MultiCastMontage_Dodge_Implementation �Լ��� ��� �Լ���

#include "Weapons/Gun.h"
#include "Weapons/CrossHair.h"
#include "Weapons/WeaponSwitch.h"

#include "Animation/AnimMontage.h" // class UAnimMontage
#include "Net/UnrealNetwork.h" // ~_Implementation(), ~_Validate(), GetLifetimeReplicatedProps()
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Characters/Actor/GnuProjectileActor.h"
#include "Characters/Actor/GnuHealActor.h"

#include "Characters/Widget/GnuCharacterBaseWidget.h"

// Character 합치면서 추가
#include "Components/WidgetComponent.h"
#include "HUD/GNUOverHeadWidget.h"
#include "Characters/GnuMyPlayerController.h"
#include "GameModes/GNUGameMode.h"

// GnuWeapon
#include "Weapons/GnuWeapon.h"
#include "Weapons/GnuCombatComponent.h"
#include "GNU_PBL/GNU_PBL.h"


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
	SetReplicateMovement(true);
	
	// -------------------- CrossHair create -----------------
	static ConstructorHelpers::FClassFinder<UUserWidget> CrossHairFinder(TEXT("/Game/GNU/weapon/UI_CrossHair.UI_CrossHair_C"));
	if (CrossHairFinder.Succeeded())
	{
		CrossHairWidgetClass = CrossHairFinder.Class;
	}
	// ---------------------------------------------------------------

	CurHP = 50.f;
	MaxHP = 100.f;
	CurStamina = 50.f;
	MaxStamina = 100.f;

	OverlapItem = nullptr;	// 공격이 맞았는지 확인

	// 추가 사항
	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	// GnuWeaponComponent
	Combat = CreateDefaultSubobject<UGnuCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	MovementComponent->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


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

	// 기존
	//if (CrossHairWidgetClass) // 크로스헤어 UI 유효성 검사
	//{
	//	// Create and add the CrossHair widget to the viewport
	//	pCrossHair = CreateWidget<UCrossHair>(GetWorld(), CrossHairWidgetClass);
	//	if (IsValid(pCrossHair))
	//	{
	//		pCrossHair->AddToViewport(); // 뷰포트 추가
	//	}
	//}

	// Bind aim rate to CrossHair
	// 기존
	/*if (AGnuMyCharacter* GnuMyCharacter = Cast<AGnuMyCharacter>(GetController() ? GetController()->GetPawn() : nullptr))
	{
		if (GnuMyCharacter)
		{
			pCrossHair->BindUserAimRate(GnuMyCharacter);
		}
	}*/

	// Gun Actor Create
	// 기존
	/*Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
	Gun->UpdateAmmoDisplay();*/
	
	  // ������ �ʱ�ȭ�ϴ� �κ�
	if (CharacterHealthWidgetClass)
	{
		CharacterHealthWidget = CreateWidget<UGnuCharacterBaseWidget>(GetWorld(), CharacterHealthWidgetClass);
		if (CharacterHealthWidget && CharacterWidget)
		{
			CharacterWidget->AddToViewport();
			CharacterHealthWidget->UpdateHealthBar(CurHP, MaxHP); // ���� �� HP ������Ʈ
		}
	}

	// 추가사항 HP, Stamina HUD 
	GNUPlayerController = GNUPlayerController == nullptr ? Cast<AGnuMyPlayerController>(Controller) : GNUPlayerController;
	if (GNUPlayerController)
	{
		GNUPlayerController->SetHUDHealth(Health, MaxHealth);
		GNUPlayerController->SetHUDStamina(Stamina, MaxStaminaa);

	}
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AGnuMyCharacter::ReceiveDamage);
	}
}

void AGnuMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ZoomInTimeline.TickTimeline(DeltaTime);
	// �� �����Ӹ��� UI ������Ʈ
	UpdateUIHealthAndStamina();
	HideCameraIfCharacterClose();

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


// ----------------------------------------- Sprint Replicate-----------------------------------
void AGnuMyCharacter::ServerSprintStart_Implementation() // Ŭ���̾�Ʈ���� ������Ʈ ���� ��û�� ������ �������� ServerSprintStart_Implementation�� ����
{
	// 기존
	//StopFire();
	UpdateSprintState(true); //  �� �Լ��� ���������� ����
	// ������ ��Ʈ��ũ���� ������ �� �ֵ��� ����
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
	// �ּ��� Ǯ�� Ŭ���̾�Ʈ���� ������Ʈ ���¸� ������ �ݿ��� �� ������, �� ����� ������ ������ ��ٸ��� �ʱ� ������ ������ Ŭ���̾�Ʈ�� �ϰ����� ������ �� ����
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

	// 기존
	/*isReload = false;*/
	// ��Ÿ�� ����
	SetDodgeMontage(Forward, Right);

	// ��Ÿ�ְ� ��ȿ�� ��� ���
	if (DodgeMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		PlayAnimMontage(DodgeMontage);
		UpdateStamina(-10.f);
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

// --------------------------------- Reload Replicate ------------------------------------

//void AGnuMyCharacter::Reload()
//{
//	if (GetController() != nullptr)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Reload!"));
//		if (pCrossHair)
//		{
//			pCrossHair->UpdateCrossHair(1); // 새로운 AimRate로 크로스헤어 업데이트
//		}
//		Gun->Reload();
//		isReload = false;
//	}
//}
//
//void AGnuMyCharacter::ServerMontageOnReload_Implementation() // �������� ������ �ִϸ��̼��� ó���ϴ� �Լ�
//{
//	if (isReload)
//	{
//		return;
//	}
//
//	isReload = true;
//	// 기존
//	//StopFire();
//	Gun->ServerMontageOnReload();
//	if (pCrossHair)
//	{
//		pCrossHair->UpdateCrossHair(3); // 새로운 AimRate로 크로스헤어 업데이트
//	}
//	if (Reload_Montage)
//	{
//		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//
//		PlayAnimMontage(Reload_Montage);
//
//		MultiCastMontage_Reload();
//	}
//}
//
//
//bool AGnuMyCharacter::ServerMontageOnReload_Validate()
//{
//	return true; // ���� ������ �ʿ��ϸ� �߰�
//}
//
//void AGnuMyCharacter::MultiCastMontage_Reload_Implementation() // ��� Ŭ���̾�Ʈ���� ������ ������ �ִϸ��̼��� ����ϵ��� �ϴ� ��Ƽĳ��Ʈ �Լ�. Ŭ���̾�Ʈ�� ���� ���� ����ȭ�� ���� ���
//{
//	if (isReload)
//	{
//		// ������ �߿� �ٽ� ȣ���ϸ� ����
//		return;
//	}
//
//	// ��Ÿ�ְ� ��ȿ�� ��� ���
//	if (Reload_Montage)
//	{
//		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//
//		PlayAnimMontage(Reload_Montage);
//
//	}
//}

//--------------------------- Arrow Skill --------------------------------
void AGnuMyCharacter::SpawnArrow()
{
	if (ArrowClass)
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
		FRotator SpawnRotation = GetActorRotation();

		AGnuProjectileActor* Arrow = GetWorld()->SpawnActor<AGnuProjectileActor>(ArrowClass, SpawnLocation, SpawnRotation);
		if (Arrow)
		{
			Arrow->LaunchProjectile(this);
		}
	}
}
// -------------------------------------------------------------------------

void AGnuMyCharacter::Aiming()
{
	if (GetController() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Aiming..."));
		if (CameraBoom->TargetArmLength > 100.0f)
		{
			CameraBoom->TargetArmLength -= 10;
		}
		// 기존
		/*if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(0.5);
		}*/
	}
}

void AGnuMyCharacter::StopAiming()
{
	if (GetController() != nullptr)
	{
		CameraBoom->TargetArmLength = 350.0f;
		// 기존
		/*if (pCrossHair)
		{
			pCrossHair->UpdateCrossHair(1);
		}*/
	}
}
// 기존
//void AGnuMyCharacter::Fire()
//{
//	
//	ServerFire();
//
//}
//
//void AGnuMyCharacter::ServerFire_Implementation()
//{
//	MultiCastFire();
//}
//
//void AGnuMyCharacter::MultiCastFire_Implementation()
//{
//	if (GetController() != nullptr)
//	{
//		if (!(isReload || isDodge))
//		{
//			if (isSprint) {
//				UpdateSprintState(false);
//			}
//			isFire = true;
//			UE_LOG(LogTemp, Warning, TEXT("fire!"));
//			if (pCrossHair)
//			{
//				pCrossHair->UpdateCrossHair(2); // 새로운 AimRate로 크로스헤어 업데이트
//
//			}
//			Gun->PullTrigger();
//		}
//	}
//}
//
//void AGnuMyCharacter::StopFire()
//{
//	ServerStopFire();
//}
//
//void AGnuMyCharacter::ServerStopFire_Implementation()
//{
//	MultiCastStopFire();
//}
//
//void AGnuMyCharacter::MultiCastStopFire_Implementation()
//{
//	if (GetController() != nullptr)
//	{
//		if (isReload != true) {
//			if (pCrossHair)
//			{
//				pCrossHair->UpdateCrossHair(1);
//			}
//		}
//		isFire = false;
//		Gun->ReleaseTrigger();
//	}
//}
//
//
//void AGnuMyCharacter::Interact()
//{
//	if (GetController() != nullptr)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Interact"));
//		AController* PlayerController = GetController();
//		if (PlayerController == nullptr)
//		{
//			return;
//		}
//
//		FVector Location;
//		FRotator Rotation;
//		PlayerController->GetPlayerViewPoint(Location, Rotation);
//
//		FVector End = Location + Rotation.Vector() * 1000;
//		// TODO: LineTrace 
//
//		FHitResult Hit;
//		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);
//
//
//		if (bSuccess)
//		{
//			// 히트된 액터가 존재하는지 확인
//			AActor* HitActor = Hit.GetActor();
//			if (HitActor)
//			{
//				// 액터에 특정 태그가 있는지 확인 (예: "WeaponSwitch")
//				if (HitActor->ActorHasTag(FName("WeaponSwitch")))
//				{
//					AWeaponSwitch* NewGun = Cast<AWeaponSwitch>(HitActor);
//					if (NewGun)
//					{
//						UE_LOG(LogTemp, Warning, TEXT("Switching Weapon..."));
//						TSubclassOf<AGun> NewGunClass = NewGun->Switching();
//						SwitchWeapon(NewGunClass);
//					}
//				}
//			}
//		}
//	}
//}
//
//void AGnuMyCharacter::SwitchWeapon(TSubclassOf<AGun> NewGunClass)
//{
//	if (NewGunClass)
//	{
//		GunClass = NewGunClass;
//
//		if (Gun)
//		{
//			Gun->RemoveAmmoDisplay();
//			Gun->Destroy();
//			Gun = nullptr;
//		}
//		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
//
//		if (Gun)
//		{
//			Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
//			Gun->SetOwner(this);
//			Gun->UpdateAmmoDisplay();
//		}
//	}
//}

bool AGnuMyCharacter::GetIsCrouching() const
{
	return isCrouch;
}

bool AGnuMyCharacter::GetIsSprinting() const
{
	return isSprint;
}

/* ----------- ���� - Ŭ���̾�Ʈ ����ȭ ���� -------------------
< ���ø����̼� ��� >
������Ʈ: isSprint ������ �������� �����ϰ�, �� ���� ������ OnRep_IsSprinting�� ���� Ŭ���̾�Ʈ�� �˸��� ����. �� Ŭ���̾�Ʈ�� �����κ��� ���������� ���¸� ����ȭ����
������ : isDodge ������ �ܼ� ���� ǥ�ÿ��̰�, ������ �ִϸ��̼��� ������ �� �� ó���ϰ� ��� Ŭ���̾�Ʈ���� ���� ����ǰԲ� ��Ƽĳ��Ʈ�� ���ĵ�

< �Լ� ȣ�� ��� >
������Ʈ: Ŭ���̾�Ʈ�� ������ ��û�� ������, ������ ���� Ŭ���̾�Ʈ���� �ٽ� ���¸� �����ϴ� ����� ��� ���
������ : Ŭ���̾�Ʈ�� ������ ��û�ϸ�, ������ ��� Ŭ���̾�Ʈ�� ���ÿ� ����� �����ϴ� �� ���� ���� ���
*/
//--------------------------- Arrow Skill --------------------------------
void AGnuMyCharacter::SpawnHeal()
{
	if (HealClass)
	{
	
		// ĳ������ ��ġ�� �������� �������� 100 ���� ������ ��ġ�� SpawnLocation ����
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 100.0f); // ���÷� ĳ������ 100 ���� ���� ����
		FRotator SpawnRotation = GetActorRotation();

		// FTransform�� ����Ͽ� ��ġ�� ȸ�� ����
		FTransform SpawnTransform(SpawnRotation, SpawnLocation);

		// HealActor ����
		AGnuHealActor* Heal = GetWorld()->SpawnActor<AGnuHealActor>(HealClass, SpawnTransform);

		if (Heal)
		{
			Heal->HealOverTime();
		}
		else
		{
			FString HPMessage = FString::Printf(TEXT("Hp Heal Spawn Error"));
		}
	}
	else
	{
		FString HPMessage = FString::Printf(TEXT("Not Heal Class!!"));
	}
}
// -------------------------------------------------------------------------


//--------------------------- HP Update --------------------------------
void AGnuMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGnuMyCharacter, isSprint);
	DOREPLIFETIME(AGnuMyCharacter, isCrouch);
	DOREPLIFETIME(AGnuMyCharacter, CurHP);
	DOREPLIFETIME(AGnuMyCharacter, Health);
	DOREPLIFETIME(AGnuMyCharacter, Stamina);

	// GnuWeapon
	DOREPLIFETIME_CONDITION(AGnuMyCharacter, OverlappingWeapon, COND_OwnerOnly);

}



void AGnuMyCharacter::OnRep_CurHP()
{
	// HP�� ����Ǹ� UI�� �ݿ�
	UpdateUIHealthAndStamina();
}

void AGnuMyCharacter::UpdateHealth(float NewHP)
{
	CurHP = FMath::Clamp(CurHP + NewHP, 0.0f, MaxHP); // HP�� 0�� MaxHP ���̷� Ŭ����
	FString HPMessage = FString::Printf(TEXT("Current HP: %f"), CurHP);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, HPMessage);
	// �������� HP ������Ʈ �� UI ������Ʈ
	if (HasAuthority())
	{
		OnRep_CurHP();
	}
}

void AGnuMyCharacter::UpdateStamina(float NewStamina)
{
	CurStamina = FMath::Clamp(CurStamina + NewStamina, 0.0f, MaxStamina);

	FString StaminaMessage = FString::Printf(TEXT("Current Stamina: %f"), CurStamina);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, StaminaMessage);
}

void AGnuMyCharacter::UpdateUIHealthAndStamina()
{
	// UI �������� ȣ��� �Լ���, Health�� Stamina �ٸ� ������Ʈ
	// ���⿡ UGnuCharacterBaseWidget�� �����ϴ� �ڵ带 ������ �˴ϴ�.
	if (CharacterHealthWidget)
	{
		CharacterHealthWidget->UpdateHealthBar(CurHP, MaxHP);
		CharacterHealthWidget->UpdateStaminaBar(CurStamina, MaxStamina);
	}
}

//
// GnuWeapon
//
void AGnuMyCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}

	}
}

void AGnuMyCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

// FireButton 눌렸을 때 Combat에 있는 FireButtonPressed 호출
// => bool 값을 넘겨서 버튼이 눌렸는지 확인 후 Fire
void AGnuMyCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AGnuMyCharacter::FireButtonReleased()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AGnuMyCharacter::ReloadButtonPressed()
{
	if (Combat)
	{
		Combat->ReloadButtonPressed();
	}
}

// Overlap 되었을 때 Widget 보이기 (F-PickUP)
void AGnuMyCharacter::SetOverlappingWeapon(AGnuWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

// Overlapping Weapon 값이 바뀔 때 마다 호출하여
// Overlap 되었을 때 보이고 안 되었을 때 안 보이게
void AGnuMyCharacter::OnRep_OverlappingWeapon(AGnuWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

// 캐릭터 할당
void AGnuMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->GnuCharacter = this;
	}
}

// 무기 장착 중인지 확인
bool AGnuMyCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

// Fire Montage 실행
void AGnuMyCharacter::PlayFireMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
	}
}

void AGnuMyCharacter::PlayReloadMontage()
{
	ServerPlayReloadMontage();
}

void AGnuMyCharacter::ServerPlayReloadMontage_Implementation()
{
	MultiCastPlayReloadMontage();
}

void AGnuMyCharacter::MultiCastPlayReloadMontage_Implementation()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadWeaponMontage)
	{
		AnimInstance->Montage_Play(ReloadWeaponMontage);
	}
}

void AGnuMyCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

AGnuWeapon* AGnuMyCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector AGnuMyCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}



// 카메라가 캐릭터에 너무 가까이 되면 캐릭터가 안 보이게 해서 시야 확보
// 근데 줌 땡기면 자동으로 가까워져서 일단 써야할 지 보류
void AGnuMyCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((TPPCamera->GetComponentLocation() - GetActorLocation()).Size() < 200.f)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AGnuMyCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	PlayHitReactMontage();
	UpdateHUDHealth();

}

void AGnuMyCharacter::UpdateHUDHealth()
{
	GNUPlayerController = GNUPlayerController == nullptr ? Cast<AGnuMyPlayerController>(Controller) : GNUPlayerController;
	if (GNUPlayerController)
	{
		GNUPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AGnuMyCharacter::UpdateHUDStamina()
{
	GNUPlayerController = GNUPlayerController == nullptr ? Cast<AGnuMyPlayerController>(Controller) : GNUPlayerController;
	if (GNUPlayerController)
	{
		GNUPlayerController->SetHUDStamina(Stamina, MaxStaminaa);
	}
}



void AGnuMyCharacter::OnRep_Health()
{
	PlayHitReactMontage();
	UpdateHUDHealth();
}

void AGnuMyCharacter::OnRep_Stamina()
{

}

void AGnuMyCharacter::ClientSetName_Implementation(const FString& Name)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->PlayerState->SetPlayerName(Name);
	}

}


void AGnuMyCharacter::ServerSetPlayerName_Implementation(const FString& PlayerName)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->PlayerState->SetPlayerName(PlayerName);
		ClientSetName(PlayerName);
	}
}