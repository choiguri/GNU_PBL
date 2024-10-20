// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" // GetCharacterMovement()
#include "GameFramework/SpringArmComponent.h" // SpringArm
#include "Camera/CameraComponent.h" // TPP, FPP Camera
#include "Characters/GnuMyCharacterAnimInstance.h" // SetDodge함수의 몇가지 함수들
#include "Animation/AnimMontage.h" // class UAnimMontage
#include "AbilitySystemComponent.h"
#include "AbilitySystems/GnuAbilitySystemComponent.h"
#include "Characters/GnuCharacterPlayerController.h"
#include "Characters/GnuCharacterPlayerState.h"
#include "UI/GnuHUD.h"

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
	CameraBoom->TargetArmLength = 300.0f; // 원하는 길이로 설정
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
	isDodging = false;
	// ---------------------------------------------------------------
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

void AGnuMyCharacter::SetSprintSpeed()
{
	UGnuMyCharacterAnimInstance* AnimInstance = Cast<UGnuMyCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (AnimInstance && MovementComponent)
	{
		AnimInstance->SwitchSprint(); // 상태 토글

		if (AnimInstance->GetIsSprinting())
		{
			MovementComponent->MaxWalkSpeed = SprintSpeed; // 스프린트 속도로 설정
		}
		else
		{
			MovementComponent->MaxWalkSpeed = DefaultSpeed; // 기본 속도로 복원
		}
	}
}

void AGnuMyCharacter::SetCrouch()
{
	UGnuMyCharacterAnimInstance* AnimInstance = Cast<UGnuMyCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInstance)
	{
		AnimInstance->SwitchCrouch(); // 상태 토글

		if (AnimInstance->GetIsCroucing())
		{
			Crouch(); // 스프린트 속도로 설정
		}
		else
		{
			UnCrouch(); // 기본 속도로 복원
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

void AGnuMyCharacter::SetDodge(float Forward, float Right)
{
	if (isDodging)
	{
		// 구르기 중에 다시 호출하면 중지
		return;
	}

	UAnimMontage* DodgeMontage = nullptr;

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
		isDodging = true; // 구르기 시작할 때 true로 설정
		AnimInstance->Montage_Play(DodgeMontage);

		// OnCompleted 및 OnInterrupted 이벤트 바인딩
		// FOnMontageEnded : 몽타주가 종료되었을 때 호출되는 델리게이트(함수 포인터) 
		// -> 애니메이션 몽타주가 완료되거나 중단될 때 연결된 함수를 호출하는 데 사용
		FOnMontageEnded MontageEndedDelegate;
		// BindUFunction : 델리게이트에 특정 함수를 바인딩하는 역할. 이 함수가 호출되면 OnMontageEnded라는 이름의 함수를 실행
		MontageEndedDelegate.BindUFunction(this, FName("OnMontageEnded"));
		//Montage_SetEndDelegate : 애니메이션 인스턴스의 몽타주가 끝났을 때 어떤 델리게이트를 호출할지를 설정
		// ===> DodgeMontage 이 몽타주가 끝나면 OnMontageEnded 이 이름을 가진 함수를 호출하는 것이 된다
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DodgeMontage);
	}
}

// 몽타주가 완료되었을 때 호출되는 함수
void AGnuMyCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	isDodging = false; // 애니메이션 완료 후 false로 설정
}

// 몽타주가 중단되었을 때 호출되는 함수
void AGnuMyCharacter::OnMontageInterrupted(UAnimMontage* Montage)
{
	isDodging = false; // 애니메이션 중단 시 false로 설정
}