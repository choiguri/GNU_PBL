// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCharacterMovement()
#include "Components/CapsuleComponent.h" // SetCapsuleHalfHeight()
#include "GameFramework/SpringArmComponent.h" // SpringArm
#include "Camera/CameraComponent.h" // TPP, FPP Camera
#include "Characters/GnuMyCharacterAnimInstance.h" // MultiCastMontage_Dodge_Implementation 함수의 몇가지 함수들
#include "Animation/AnimMontage.h" // class UAnimMontage
#include "Net/UnrealNetwork.h" // ~_Implementation(), ~_Validate(), GetLifetimeReplicatedProps()
#include "AbilitySystemComponent.h"
#include "AbilitySystems/GnuAbilitySystemComponent.h"
#include "Characters/GnuCharacterPlayerController.h"
#include "Characters/GnuCharacterPlayerState.h"
#include "UI/GnuHUD.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AGnuMyCharacter::AGnuMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ------------------------------- 캐릭터 회전 ------------------------------
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 400.f, 0.f);
	// ---------------------------------------------------------------

	// ------------------------------- Camera 초기화 ------------------------------
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); // RootComponent에 부착
	CameraBoom->TargetArmLength = 350.0f; // 원하는 길이로 설정
	CameraBoom->SocketOffset = FVector(0.0f, 80.0f, 40.0f);
	CameraBoom->bUsePawnControlRotation = true; // 카메라 회전 사용

	TPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPPCamera"));
	TPPCamera->SetupAttachment(CameraBoom); // 카메라붐에 부착
	TPPCamera->bUsePawnControlRotation = false;

	FPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPPCamera"));
	FPPCamera->SetupAttachment(GetMesh(), TEXT("Head")); // Head 소켓에 부착
	FPPCamera->bUsePawnControlRotation = true;

	isFPPCamera = false;
	// ---------------------------------------------------------------

	// ---------------------- Movement 관련 초기화 -----------------------
	DefaultSpeed = 300.0f;
	SprintSpeed = 600.0f;
	MovementComponent->MaxWalkSpeed = DefaultSpeed;
	bReplicates = true;
	isSprint = false;
	isDodge = false;
	isCrouch = false;
	isZoomIn = false;
	DodgeMontage = nullptr;
	CrouchSmoothCurve = nullptr;
	// ---------------------------------------------------------------

	// ------------------- 캡슐 컴포넌트 높이 초기화 -------------------
	CrouchedCapsuleHalfHeight = 60.0f;
	StandingCapsuleHalfHeight = 80.0f;
	// ---------------------------------------------------------------

	isWeaponEquip = false;
	isPistolEquip = false;
	isRifleEquip = false;
}

void AGnuMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ---------------- Crouch 카메라 암 부드럽게 조절되는 타임라인 초기화 ----------
	FOnTimelineFloat CrouchProgressUpdate;
	CrouchProgressUpdate.BindUFunction(this, FName("CrouchUpdate"));

	FOnTimelineEvent CrouchFinishedEvent;
	CrouchFinishedEvent.BindUFunction(this, FName("CrouchFinished"));

	CrouchSmoothTimeline.AddInterpFloat(CrouchSmoothCurve, CrouchProgressUpdate);
	CrouchSmoothTimeline.SetTimelineFinishedFunc(CrouchFinishedEvent);
	CrouchSmoothTimeline.SetTimelineLength(1.0f);
	// -------------------------------------------------------------------------------

	// ---------------- ZoomIn 부드럽게 조절되는 타임라인 초기화 ----------------
	FOnTimelineFloat ZoomInProgressUpdate;
	ZoomInProgressUpdate.BindUFunction(this, FName("ZoomInUpdate"));

	FOnTimelineEvent ZoomInFinishedEvent;
	ZoomInFinishedEvent.BindUFunction(this, FName("ZoomInFinished"));

	ZoomInTimeline.AddInterpFloat(ZoomInCurve, ZoomInProgressUpdate);
	ZoomInTimeline.SetTimelineFinishedFunc(ZoomInFinishedEvent);
	ZoomInTimeline.SetTimelineLength(0.3f);
	// -------------------------------------------------------------------------------
}

void AGnuMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CrouchSmoothTimeline.TickTimeline(DeltaTime);
	ZoomInTimeline.TickTimeline(DeltaTime);
}

void AGnuMyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
}
void AGnuMyCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

void AGnuMyCharacter::InitAbilityActorInfo()
{
	AGnuCharacterPlayerState* GnuPlayerState = GetPlayerState<AGnuCharacterPlayerState>();
	check(GnuPlayerState);
	GnuPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(GnuPlayerState, this);
	Cast<UGnuAbilitySystemComponent>(GnuPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = GnuPlayerState->GetAbilitySystemComponent();
	AttributeSet = GnuPlayerState->GetAttributeSet();

	if (AGnuCharacterPlayerController* GnuPlayerController = Cast<AGnuCharacterPlayerController>(GetController()))
	{
		if (AGnuHUD* GnuHUD = Cast<AGnuHUD>(GnuPlayerController->GetHUD()))
		{
			GnuHUD->InitOverlay(GnuPlayerController, GnuPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

void AGnuMyCharacter::SetCamera()
{
	isFPPCamera = !isFPPCamera; // 현재 상태를 반전시킴

	if (isFPPCamera)
	{
		// 1인칭 카메라 활성화
		if (FPPCamera && TPPCamera) // Null 체크
		{
			TPPCamera->SetActive(false);
			FPPCamera->SetActive(true);
		}
	}
	else
	{
		// 3인칭 카메라 활성화
		if (FPPCamera && TPPCamera) // Null 체크
		{
			TPPCamera->SetActive(true);
			FPPCamera->SetActive(false);
		}
	}
}

void AGnuMyCharacter::ZoomInUpdate(float Alpha)
{
	float NewArmLength = 0.0f;
	
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
		if (ZoomInCurve) // Null 체크
		{
			ZoomInTimeline.ReverseFromEnd();
		}
	}
}

void AGnuMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 변수를 네트워크에서 복제할 수 있도록 설정
	DOREPLIFETIME(AGnuMyCharacter, isSprint);
	DOREPLIFETIME(AGnuMyCharacter, isCrouch);
}

// ----------------------------------------- Sprint Replicate-----------------------------------
void AGnuMyCharacter::ServerSprintStart_Implementation()
{
	UpdateSprintState(true);
}

bool AGnuMyCharacter::ServerSprintStart_Validate()
{
	return true; // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ServerSprintEnd_Implementation()
{
	UpdateSprintState(false);
}

bool AGnuMyCharacter::ServerSprintEnd_Validate()
{
	return true; // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ClientSprintStart_Implementation()
{
	// 클라이언트에서도 스프린트 상태와 속도 설정
	//UpdateSprintState(true);
}

void AGnuMyCharacter::ClientSprintEnd_Implementation()
{
	// 클라이언트에서도 스프린트 상태와 속도 초기화
	//UpdateSprintState(false);
}

void AGnuMyCharacter::OnRep_IsSprinting()
{
	// 클라이언트가 동기화 되는 함수
	// Client의 Implementation 함수에서만 써도 동일한 기능이 작동된다.
	// 다만 Clint 함수 내부에서 사용하면 좀 더 반응 속도가 빠른 반면, OnRep 함수에서 쓰면 서버와 클라이언트간의 일관성을 유지할 수 있다.
	UpdateSprintState(isSprint);
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

void AGnuMyCharacter::CrouchUpdate(float Alpha)
{
	float NewArmLength = FMath::Lerp(250.0f, 350.0f, Alpha);

	// CameraBoom의 TargetArmLength를 업데이트
	CameraBoom->TargetArmLength = NewArmLength;
}

void AGnuMyCharacter::CrouchFinished()
{

}

void AGnuMyCharacter::ServerCrouchStart_Implementation()
{
	isCrouch = true;
	//UpdateCapsuleSize(); // 앉을 때 캡슐 크기 업데이트

	if (CrouchSmoothCurve)
	{
		CrouchSmoothTimeline.PlayFromStart(); // CrouchSmoothCurve를 사용하여 타임라인 시작
	}

}

bool AGnuMyCharacter::ServerCrouchStart_Validate()
{
	return true; // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ServerCrouchEnd_Implementation()
{
	//FVector ActorLocation = GetActorLocation();
	//FRotator ActorRotation = GetActorRotation();
	//FVector UpVector = UKismetMathLibrary::GetUpVector(ActorRotation);
	//FVector Result = UpVector * 100.0f;
	//FVector End = ActorLocation + Result;

	//FHitResult HitResult;
	//FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(this); // 자기 자신을 무시

	//// 위에 무언가 막혀있으면 일어나지 못하게 막는 기능
	//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ActorLocation, End, ECC_Visibility, CollisionParams);

	//if (!bHit)
	//{
	//	isCrouch = false;
	//	UpdateCapsuleSize(); // 앉을 때 캡슐 크기 업데이트
	//}
	isCrouch = false;

	CrouchSmoothTimeline.ReverseFromEnd(); // CrouchSmoothCurve를 사용하여 타임라인 시작
}

bool AGnuMyCharacter::ServerCrouchEnd_Validate()
{
	return true; // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ClientCrouchStart_Implementation()
{
	isCrouch = true;
}

void AGnuMyCharacter::ClientCrouchEnd_Implementation()
{
	isCrouch = false;
}

void AGnuMyCharacter::OnRep_IsCrouching()
{
	// 앉기 상태에 따라 상태 조절
	if (isSprint)
	{
		Crouch();
		//UpdateCapsuleSize(); // 앉을 때 캡슐 크기 업데이트
	}
	else
	{
		UnCrouch();
		//UpdateCapsuleSize(); // 앉을 때 캡슐 크기 업데이트
	}
}

void AGnuMyCharacter::UpdateCapsuleSize()
{
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(GetCapsuleComponent());
	if (Capsule)
	{
		if (isCrouch)
		{
			// 앉았을 때 캡슐 크기 설정
			Capsule->SetCapsuleHalfHeight(CrouchedCapsuleHalfHeight);
		}
		else
		{
			// 서 있을 때 캡슐 크기 원래대로 설정
			Capsule->SetCapsuleHalfHeight(StandingCapsuleHalfHeight);
		}
	}
}
// -------------------------------------------------------------------------


// --------------------------------- Dodge Replicate ------------------------------------
void AGnuMyCharacter::MultiCastMontage_Dodge_Implementation(float Forward, float Right)
{
	if (isDodge)
	{
		// 구르기 중에 다시 호출하면 중지
		return;
	}

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

	// 몽타주가 유효한 경우 재생
	if (DodgeMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		//isDodge = true; // 구르기 시작할 때 true로 설정
		//AnimInstance->Montage_Play(DodgeMontage);
		PlayAnimMontage(DodgeMontage);
		// 멀티캐스트 호출하여 모든 클라이언트에서 구르기 애니메이션 실행
		//// OnCompleted 및 OnInterrupted 이벤트 바인딩
		//// FOnMontageEnded : 몽타주가 종료되었을 때 호출되는 델리게이트(함수 포인터) 
		//// -> 애니메이션 몽타주가 완료되거나 중단될 때 연결된 함수를 호출하는 데 사용
		//FOnMontageEnded MontageEndedDelegate;
		//// BindUFunction : 델리게이트에 특정 함수를 바인딩하는 역할. 이 함수가 호출되면 OnMontageEnded라는 이름의 함수를 실행
		//MontageEndedDelegate.BindUFunction(this, FName("OnMontageEnded"));
		////Montage_SetEndDelegate : 애니메이션 인스턴스의 몽타주가 끝났을 때 어떤 델리게이트를 호출할지를 설정
		//// ===> DodgeMontage 이 몽타주가 끝나면 OnMontageEnded 이 이름을 가진 함수를 호출하는 것이 된다
		//AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DodgeMontage);
	}
}

void AGnuMyCharacter::ServerMontageOnDodge_Implementation(float Forward, float Right)
{
	MultiCastMontage_Dodge(Forward, Right);
}

// 서버에서 구르기 몽타주 실행 검증
bool AGnuMyCharacter::ServerMontageOnDodge_Validate(float Forward, float Right)
{
	return true; // 검증 로직이 필요하면 추가
}
// -------------------------------------------------------------------------


void AGnuMyCharacter::SetPrimaryWeapons()
{
	if (!isRifleEquip)
	{
		isWeaponEquip = true;
		isPistolEquip = true;
	}
}


// 몽타주가 완료되었을 때 호출되는 함수
//void AGnuMyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
//{
//	isDodge = false; // 애니메이션 완료 후 false로 설정
//}
//
//// 몽타주가 중단되었을 때 호출되는 함수
//void AGnuMyCharacter::OnMontageInterrupted(UAnimMontage* Montage)
//{
//	isDodge = false; // 애니메이션 중단 시 false로 설정
//}