// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCharacterMovement()
#include "Components/CapsuleComponent.h" // SetCapsuleHalfHeight()
#include "GameFramework/SpringArmComponent.h" // SpringArm
#include "Camera/CameraComponent.h" // TPP, FPP Camera

// Monster
#include "Monster/AttackActor/GnuFireballActor.h"
#include "Monster/AttackActor/GnuFirebreathActor.h"	
#include "Monster/AttackActor/GnuFiretornadoActor.h"
#include "Monster/AttackActor/GnuGroundActor.h"
#include "Monster/AttackActor/GnuGroundSpikeCollisionActor.h"
#include "Camera/PlayerCameraManager.h" // Shake Camera (intro Action)

// Weapon
#include "Weapons/Gun.h"
#include "Weapons/CrossHair.h"
#include "Weapons/WeaponSwitch.h"

// Animation
#include "Animation/AnimMontage.h" // class UAnimMontage
#include "Characters/GnuMyAnimInstance.h"

// Multi
#include "Net/UnrealNetwork.h" // ~_Implementation(), ~_Validate(), GetLifetimeReplicatedProps()

// Math
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Skill
#include "Characters/Actor/GnuProjectileActor.h"
#include "Characters/Actor/GnuHealActor.h"
#include "Characters/Actor/GnuGrenadeActor.h"

// Character 합치면서 추가
#include "Components/WidgetComponent.h"
#include "HUD/GNUOverHeadWidget.h"
#include "Characters/GnuMyPlayerController.h"
#include "HUD/GnuReplicatedHealth.h"
#include "Components/ProgressBar.h"

// GnuWeapon
#include "Weapons/GnuWeapon.h"
#include "Weapons/GnuCombatComponent.h"
#include "GNU_PBL/GNU_PBL.h"


// GameMode
#include "GameModes/GNUGameMode.h"
#include "TimerManager.h"

AGnuMyCharacter::AGnuMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ------------------------------- Character Rotation ------------------------------
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 400.f, 0.f);
	// ---------------------------------------------------------------

	// ------------------------------- Camera Init ------------------------------
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 80.0f, 40.0f);
	CameraBoom->bUsePawnControlRotation = true; // 카메라 회전 사용

	TPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPPCamera"));
	TPPCamera->SetupAttachment(CameraBoom);
	TPPCamera->bUsePawnControlRotation = false;

	FPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPPCamera"));
	FPPCamera->SetupAttachment(GetMesh(), TEXT("Head")); // Head 소켓에 부착
	FPPCamera->bUsePawnControlRotation = true;

	isFPPCamera = false;
	// ---------------------------------------------------------------

	// ---------------------- Movement Init -----------------------
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
	static ConstructorHelpers::FClassFinder<UUserWidget> CrossHairFinder(TEXT("/Game/GNU/Character/weapon/UI_CrossHair.UI_CrossHair_C"));
	if (CrossHairFinder.Succeeded())
	{
		CrossHairWidgetClass = CrossHairFinder.Class;
	}
	// ---------------------------------------------------------------


	OverlapItem = nullptr;	// 공격이 맞았는지 확인

	// 추가 사항
	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	ReplicatedHealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ReplicatedHealthWidget"));
	ReplicatedHealthWidget->SetupAttachment(RootComponent);

	// GnuWeaponComponent
	Combat = CreateDefaultSubobject<UGnuCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	MovementComponent->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	/*GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);*/
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	isHealCoolDown = false;
}

void AGnuMyCharacter::PlayCameraShake()
{
	if (RoarCameraShake)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(RoarCameraShake);
	}
}

void AGnuMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ---------------- ZoomIn 부드럽게 조절되는 타임라인 초기화 ----------------
	// FOnTimelineFloat : 타임라인에서 플로트(float) 값을 반환하는 델리게이트 타입
	FOnTimelineFloat ZoomInProgressUpdate;
	// BindUFunction : ZoomInUpdate이라는 이름을 가진 함수를 바인딩
	// 타임라인이 진행될 때마다 ZoomInUpdate 함수를 호출
	ZoomInProgressUpdate.BindUFunction(this, FName("ZoomInUpdate"));

	// FOnTimelineEvent : 타임라인이 끝났을 때 호출되는 이벤트 델리게이트
	FOnTimelineEvent ZoomInFinishedEvent;
	// ZoomInFinished이라는 이름을 가진 함수를 바인딩
	ZoomInFinishedEvent.BindUFunction(this, FName("ZoomInFinished"));

	// AddInterpFloat : 플로트 곡선(Curve)을 추가, 해당 곡선을 따라 값을 변화시키면서 델리게이트(ZoomInProgressUpdate)를 호출
	ZoomInTimeline.AddInterpFloat(ZoomInCurve, ZoomInProgressUpdate);
	// SetTimelineFinishedFunc : 타임라인이 끝났을 때 호출할 델리게이트(이벤트)를 설정
	ZoomInTimeline.SetTimelineFinishedFunc(ZoomInFinishedEvent);
	// SetTimelineLength : 타임라인 길이 설정
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

	// 추가사항 HP, Stamina HUD 
	GNUPlayerController = GNUPlayerController == nullptr ? Cast<AGnuMyPlayerController>(Controller) : GNUPlayerController;
	if (GNUPlayerController)
	{
		GNUPlayerController->SetHUDHealth(Health, MaxHealth);
		GNUPlayerController->SetHUDStamina(Stamina, MaxStaminaa);

	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("No PlayerController")));
		}
	}
	if (ReplicatedHealthWidget)
	{
		UGnuReplicatedHealth* HealthWidget = Cast<UGnuReplicatedHealth>(ReplicatedHealthWidget->GetWidget());
		if (HealthWidget)
		{
			const float HealthPercent = Health / MaxHealth;
			HealthWidget->ReplicatedHealth->SetPercent(HealthPercent);
			FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
			HealthWidget->ReplicatedHealthText->SetText(FText::FromString(HealthText));
		}
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
	HideCameraIfCharacterClose();
}

void AGnuMyCharacter::SetCamera()
{
	isFPPCamera = !isFPPCamera;// 현재 상태를 반전시킴

	if (isFPPCamera)
	{
		// 1인칭 카메라 활성화
		if (FPPCamera && TPPCamera) // Null Check
		{
			TPPCamera->SetActive(false);
			FPPCamera->SetActive(true);
		}
	}
	else
	{
		// 3인칭 카메라 활성화
		if (FPPCamera && TPPCamera)
		{
			TPPCamera->SetActive(true);
			FPPCamera->SetActive(false);
		}
	}
}


// ------------------------ 카메라 줌인 줌 아웃 -----------------------
void AGnuMyCharacter::ZoomInUpdate(float Alpha)
{
	float NewArmLength = 0.0f;

	// Lerp : 두 값 사이를 선형적으로 보간하는 함수
	// Alpha : 0 ~ 1사이 값으로 보간이 얼마나 진행되었는지 나타냄
	NewArmLength = FMath::Lerp(350.0f, 150.0f, Alpha);
	FVector StartOffset(0.0f, 80.0f, 40.0f);
	FVector EndOffset(0.0f, 80.0f, 60.0f);

	// CameraBoom의 StartOffset을 업데이트
	FVector NewSocketOffset = FMath::Lerp(StartOffset, EndOffset, Alpha);
	CameraBoom->SocketOffset = NewSocketOffset;
	// CameraBoom의 TargetArmLength를 업데이트
	CameraBoom->TargetArmLength = NewArmLength;
}

void AGnuMyCharacter::ZoomInFinished()
{

}

void AGnuMyCharacter::SetZoomIn()
{
	isZoomIn = !isZoomIn; // 현재 상태를 반전시킴
	
	if (isZoomIn)
	{
		if (ZoomInCurve) // Null 체크
		{
			ZoomInTimeline.PlayFromStart();
		}
	}
	else
	{
		if (ZoomInCurve)
		{
			ZoomInTimeline.ReverseFromEnd();
		}
	}
}
// -----------------------------------------------------------------


// ----------------------------------------- Sprint Replicate-----------------------------------
void AGnuMyCharacter::ServerSprintStart_Implementation() // 클라이언트에서 스프린트 시작 요청을 보내면 서버에서 ServerSprintStart_Implementation이 실행
{
	// 기존
	//StopFire();
	/*if (!HasAuthority())
	{
		ClientSprintStart();

	}*/
	UpdateSprintState(true); //  �� �Լ��� ���������� ����
	// ������ ��Ʈ��ũ���� ������ �� �ֵ��� ����
	
}

bool AGnuMyCharacter::ServerSprintStart_Validate()
{
	return true;  // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ServerSprintEnd_Implementation()
{
	
	/*if (!HasAuthority())
	{
		ClientSprintEnd();

	}*/
	UpdateSprintState(false);

}

bool AGnuMyCharacter::ServerSprintEnd_Validate()
{
	return true; // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ClientSprintStart_Implementation() // 클라이언트에 즉시 반영하는 방식이지만 서버 - 클라이언트 일관성이 떨어짐
{
	// 주석을 풀면 클라이언트에서 스프린트 상태를 빠르게 반영할 수 있지만, 이 방식은 서버의 명령을 기다리지 않기 때문에 서버와 클라이언트의 일관성이 떨어질 수 있음
	 UpdateSprintState(true);
	
}

void AGnuMyCharacter::ClientSprintEnd_Implementation()
{

	 UpdateSprintState(false);
}

void AGnuMyCharacter::OnRep_IsSprinting()// 클라이언트가 동기화 되는 함수 -> 서버에서 isSprint 값이 변경될 때 클라이언트에서 그 변화를 감지하고 실행되는 함수 (즉, 서버가 관리하는 스프린트 상태를 클라이언트가 동기화 하는 방식)
{
	// Client의 Implementation 함수에서만 써도 동일한 기능이 작동된다.
	// -> 다만 Clint 함수 내부에서 사용하면 좀 더 반응 속도가 빠른 반면, OnRep 함수에서 쓰면 서버와 클라이언트간의 일관성을 유지할 수 있다.
	UpdateSprintState(isSprint); // 스프린트 상태가 변경된 것을 클라이언트에서 반영
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
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed; // 기본 속도 설정
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
	return true;
}

void AGnuMyCharacter::ServerCrouchEnd_Implementation()
{
	isCrouch = false;
}

bool AGnuMyCharacter::ServerCrouchEnd_Validate()
{
	return true;
}

void AGnuMyCharacter::ClientCrouchStart_Implementation()
{
}

void AGnuMyCharacter::ClientCrouchEnd_Implementation()
{
}

void AGnuMyCharacter::OnRep_IsCrouching()
{
	// 앉기 상태에 따라 조절
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
void AGnuMyCharacter::ServerMontageOnDodge_Implementation(float Forward, float Right)// 서버에서 구르기 애니메이션을 처리하는 함수
{
	if (isDodge)
	{
		// 구르기 중에 다시 호출하면 중지
		return;
	}

	// 기존
	/*isReload = false;*/
	// ��Ÿ�� ����
	SetDodgeMontage(Forward, Right);

	// 몽타주가 유효한 경우 재생
	if (DodgeMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		PlayAnimMontage(DodgeMontage);
		// 멀티캐스트 호출
		MultiCastMontage_Dodge(Forward, Right);
	}
}

bool AGnuMyCharacter::ServerMontageOnDodge_Validate(float Forward, float Right)
{
	return true;
}

void AGnuMyCharacter::MultiCastMontage_Dodge_Implementation(float Forward, float Right) // 모든 클라이언트에서 동일한 구르기 애니메이션을 재생하도록 하는 멀티캐스트 함수. 클라이언트와 서버 간의 동기화를 위해 사용
{
	if (isDodge)
	{
		return;
	}

	// 몽타주 설정
	SetDodgeMontage(Forward, Right);

	// 몽타주가 유효한 경우 재생
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
		// Forward가 0인 경우, Right를 확인
		if (Right != 0)
		{
			// Right가 0보다 크면 DiveRoll_R_Montage, 그렇지 않으면 DiveRoll_L_Montage 저장
			DodgeMontage = (Right > 0) ? DiveRoll_R_Montage : DiveRoll_L_Montage;
		}
	}
	else
	{
		// Forward가 0보다 크면 DiveRoll_F_Montage, 그렇지 않으면 DiveRoll_B_Montage 저장
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

//--------------------------- Heal Skill --------------------------------
void AGnuMyCharacter::SpawnHeal()
{
	if (HealClass)
	{
		if (!isHealCoolDown)
		{
			FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 500.0f);
			FRotator SpawnRotation = GetActorRotation();

			FTransform SpawnTransform(SpawnRotation, SpawnLocation);

			AGnuHealActor* Heal = GetWorld()->SpawnActor<AGnuHealActor>(HealClass, SpawnTransform);

			if (Heal)
			{
				StartCooldown();
				Heal->SetOwner(this); // 이거 안해주면 GnuHealActor에서 onwer가 누군지 몰라서 오류가 난다
			}
		}
	}
}

void AGnuMyCharacter::StartCooldown()
{
	isHealCoolDown = true;
	GetWorld()->GetTimerManager().SetTimer(HealCoolDownTimer, this, &AGnuMyCharacter::EndCooldown, 10.0f, false);
}

void AGnuMyCharacter::EndCooldown()
{
	isHealCoolDown = false; // 쿨타임 해제
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Heal Cooldown ended!"));
}

// -------------------------------------------------------------------------

//--------------------------- Arrow Skill --------------------------------
void AGnuMyCharacter::SpawnArrow()
{
	if (ArrowClass && GetEquippedWeapon())
	{
		FVector SpawnLocation = Combat->EquippedWeapon->GetMesh()->GetSocketLocation("MuzzleFlashSocket");
		FRotator SpawnRotation = Combat->EquippedWeapon->GetMesh()->GetSocketRotation("MuzzleFlashSocket");
		FTransform SpawnTransform(SpawnRotation, SpawnLocation);
			
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = this;
		UWorld* World = GetWorld();
		if (World)
		{
			AGnuProjectileActor* Arrow = World->SpawnActor<AGnuProjectileActor>(ArrowClass, SpawnTransform, SpawnParams);
			if (Arrow)
			{
				Arrow->SetOwner(this);
				Arrow->LaunchProjectile(this);
			}
		}
	}
}
// -------------------------------------------------------------------------

//--------------------------- Grenade Skill --------------------------------
void AGnuMyCharacter::SpawnGrenade()
{
	if (GrenadeClass)
	{
		FVector SpawnLocation = Combat->EquippedWeapon->GetMesh()->GetSocketLocation("MuzzleFlashSocket");
		FRotator SpawnRotation = Combat->EquippedWeapon->GetMesh()->GetSocketRotation("MuzzleFlashSocket");
		FTransform SpawnTransform(SpawnRotation, SpawnLocation);

		AGnuGrenadeActor* Grenade = GetWorld()->SpawnActor<AGnuGrenadeActor>(GrenadeClass, SpawnTransform);
		if (Grenade)
		{
			Grenade->SetOwner(this);
			Grenade->LaunchProjectile(this, SpawnLocation, SpawnRotation);
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
//
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




void AGnuMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 변수를 네트워크에서 복제할 수 있도록 설정
	DOREPLIFETIME(AGnuMyCharacter, isSprint);
	DOREPLIFETIME(AGnuMyCharacter, isCrouch);
	DOREPLIFETIME(AGnuMyCharacter, Health);
	DOREPLIFETIME(AGnuMyCharacter, Stamina);

	// GnuWeapon
	DOREPLIFETIME_CONDITION(AGnuMyCharacter, OverlappingWeapon, COND_OwnerOnly);

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
	// 재장전 중이면 사격 불가
	if (!Combat || Combat->bReloadButtonPressed || GetEquippedWeapon() == nullptr) return;

	// 뛰고 있으면 걷는 상태로 바꾼 후 사격
	//if (isSprint)
	//{
	//	//UpdateSprintState(false);
	//	ServerSprintEnd();
	//}
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
		Combat->ReloadButtonPressed(true);
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

void AGnuMyCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
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

	if (Health == 0.f)
	{
		AGNUGameMode* GnuGameMode = GetWorld()->GetAuthGameMode<AGNUGameMode>();
		if (GnuGameMode)
		{
			GNUPlayerController = GNUPlayerController == nullptr ? Cast<AGnuMyPlayerController>(Controller) : GNUPlayerController;
			AController* MonsterController = Cast<AController>(InstigatorController);
			GnuGameMode->PlayerEliminated(this, GNUPlayerController, MonsterController);
		}
	}
}

void AGnuMyCharacter::UpdateHUDHealth()
{
	GNUPlayerController = GNUPlayerController == nullptr ? Cast<AGnuMyPlayerController>(Controller) : GNUPlayerController;
	if (GNUPlayerController)
	{
		GNUPlayerController->SetHUDHealth(Health, MaxHealth);
	}
	
	SetReplicatedHealth();
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


void AGnuMyCharacter::SetReplicatedHealth()
{
	MultiCastSetHealth();
}

void AGnuMyCharacter::MultiCastSetHealth_Implementation()
{
	if (ReplicatedHealthWidget)
	{
		UGnuReplicatedHealth* HealthWidget = Cast<UGnuReplicatedHealth>(ReplicatedHealthWidget->GetWidget());
		if (HealthWidget)
		{
			const float HealthPercent = Health / MaxHealth;
			HealthWidget->ReplicatedHealth->SetPercent(HealthPercent);
			FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
			HealthWidget->ReplicatedHealthText->SetText(FText::FromString(HealthText));
			
		}
	}
}

void AGnuMyCharacter::Elim()
{
	MultiCastElim();

	// GameMode는 서버에서만 값을 가짐 
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&AGnuMyCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void AGnuMyCharacter::MultiCastElim_Implementation()
{
	PlayElimMontage();
}

void AGnuMyCharacter::ElimTimerFinished()
{
	AGNUGameMode* GnuGameMode = GetWorld()->GetAuthGameMode<AGNUGameMode>();
	if (GnuGameMode)
	{
		GnuGameMode->RequestRespawn(this, Controller);
	}
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


/* ----------- 서버 - 클라이언트 동기화 설명 -------------------
< 리플리케이션 방식 >
스프린트: isSprint 변수를 서버에서 관리하고, 그 변동 사항을 OnRep_IsSprinting을 통해 클라이언트에 알리는 구조. 각 클라이언트가 서버로부터 개별적으로 상태를 동기화받음
구르기 : isDodge 변수는 단순 상태 표시용이고, 구르기 애니메이션은 서버가 한 번 처리하고 모든 클라이언트에서 동시 실행되게끔 멀티캐스트로 전파됨

< 함수 호출 방식 >
스프린트: 클라이언트가 서버에 요청을 보내면, 서버는 개별 클라이언트에게 다시 상태를 전파하는 양방향 통신 방식
구르기 : 클라이언트가 서버에 요청하면, 서버가 모든 클라이언트에 동시에 명령을 전파하는 한 번의 전파 방식
------------------------------------------------------------- */
