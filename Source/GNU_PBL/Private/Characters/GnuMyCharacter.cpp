// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacter.h"

#include "GameFramework/CharacterMovementComponent.h" // GetCharacterMovement()
#include "Components/CapsuleComponent.h" // SetCapsuleHalfHeight()
#include "GameFramework/SpringArmComponent.h" // SpringArm
#include "Camera/CameraComponent.h" // TPP, FPP Camera
#include "Characters/GnuMyCharacterAnimInstance.h" // MultiCastMontage_Dodge_Implementation 함수의 몇가지 함수들 
#include "Animation/AnimMontage.h" // class UAnimMontage
#include "Net/UnrealNetwork.h" // ~_Implementation(), ~_Validate(), GetLifetimeReplicatedProps()
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Characters/Actor/GnuProjectileActor.h"
#include "GameFramework/PlayerController.h" // IsLocalPlayerController
#include "Characters/Widget/GnuCharacterBaseWidget.h"

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
	// ---------------------------------------------------------------

	CurHP = 50.f;
	MaxHP = 100.f;
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

	  // 위젯을 초기화하는 부분
	if (CharacterHealthWidgetClass)
	{
		CharacterHealthWidget = CreateWidget<UGnuCharacterBaseWidget>(GetWorld(), CharacterHealthWidgetClass);
		if (CharacterHealthWidget && CharacterWidget)
		{
			CharacterWidget->AddToViewport();
			CharacterHealthWidget->UpdateHealthBar(CurHP, MaxHP); // 시작 시 HP 업데이트
		}
	}

}

void AGnuMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ZoomInTimeline.TickTimeline(DeltaTime);
	// 매 프레임마다 UI 업데이트
	UpdateUIHealthAndStamina();
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
		if (ZoomInCurve) // Null 체크
		{
			ZoomInTimeline.ReverseFromEnd();
		}
	}
}
// -----------------------------------------------------------------


void AGnuMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 변수를 네트워크에서 복제할 수 있도록 설정
	DOREPLIFETIME(AGnuMyCharacter, isSprint);
	DOREPLIFETIME(AGnuMyCharacter, isCrouch);
	DOREPLIFETIME(AGnuMyCharacter, CurHP);
}

// ----------------------------------------- Sprint Replicate-----------------------------------
void AGnuMyCharacter::ServerSprintStart_Implementation() // 클라이언트에서 스프린트 시작 요청을 보내면 서버에서 ServerSprintStart_Implementation이 실행
{
	UpdateSprintState(true); //  이 함수는 서버에서만 실행
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

void AGnuMyCharacter::ClientSprintStart_Implementation() // 클라이언트에 즉시 반영하는 방식이지만 서버 - 클라이언트 일관성이 떨어짐
{
	// 주석을 풀면 클라이언트에서 스프린트 상태를 빠르게 반영할 수 있지만, 이 방식은 서버의 명령을 기다리지 않기 때문에 서버와 클라이언트의 일관성이 떨어질 수 있음
	// UpdateSprintState(true);
}

void AGnuMyCharacter::ClientSprintEnd_Implementation()
{

	// UpdateSprintState(false);
}

void AGnuMyCharacter::OnRep_IsSprinting() 	// 클라이언트가 동기화 되는 함수 -> 서버에서 isSprint 값이 변경될 때 클라이언트에서 그 변화를 감지하고 실행되는 함수 (즉, 서버가 관리하는 스프린트 상태를 클라이언트가 동기화 하는 방식)
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
	return true; // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ServerCrouchEnd_Implementation()
{
	isCrouch = false;
}

bool AGnuMyCharacter::ServerCrouchEnd_Validate()
{
	return true; // 필요한 유효성 검사 로직을 여기에 추가
}

void AGnuMyCharacter::ClientCrouchStart_Implementation()
{
}

void AGnuMyCharacter::ClientCrouchEnd_Implementation()
{
}

void AGnuMyCharacter::OnRep_IsCrouching()
{
	// 앉기 상태에 따라 상태 조절
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
void AGnuMyCharacter::ServerMontageOnDodge_Implementation(float Forward, float Right) // 서버에서 구르기 애니메이션을 처리하는 함수
{
	if (isDodge)
	{
		// 구르기 중에 다시 호출하면 중지
		return;
	}

	// 몽타주 설정
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
	return true; // 검증 로직이 필요하면 추가
}

void AGnuMyCharacter::MultiCastMontage_Dodge_Implementation(float Forward, float Right) // 모든 클라이언트에서 동일한 구르기 애니메이션을 재생하도록 하는 멀티캐스트 함수. 클라이언트와 서버 간의 동기화를 위해 사용
{
	if (isDodge)
	{
		// 구르기 중에 다시 호출하면 중지
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

void AGnuMyCharacter::OnRep_CurHP()
{
	// HP가 변경되면 UI에 반영
	UpdateUIHealthAndStamina();
}

void AGnuMyCharacter::UpdateHealth(float NewHP)
{
	CurHP = FMath::Clamp(CurHP + NewHP, 0.0f, MaxHP); // HP를 0과 MaxHP 사이로 클램핑

	// 서버에서 HP 업데이트 시 UI 업데이트
	if (HasAuthority())
	{
		OnRep_CurHP();
	}
}

void AGnuMyCharacter::UpdateUIHealthAndStamina()
{
	// UI 위젯에서 호출될 함수로, Health와 Stamina 바를 업데이트
	// 여기에 UGnuCharacterBaseWidget을 연결하는 코드를 넣으면 됩니다.
	if (CharacterHealthWidget)
	{
		CharacterHealthWidget->UpdateHealthBar(CurHP, MaxHP);
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