// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"
#include "Monster/GnuMonsterAIController.h"
// 보스 공격 관련
#include "Monster/AttackActor/GnuFireballActor.h"
#include "Monster/AttackActor/GnuFiretornadoActor.h"
#include "Monster/AttackActor/GnuFirebreathActor.h"
#include "Monster/AttackActor/GnuGroundActor.h"
#include "Monster/AttackActor/GnuGroundSpikeActor.h"
#include "Monster/AttackActor/GnuGroundSpikeCollisionActor.h"
#include "Monster/AttackActor/GnuLavaBurstActor.h"
#include "Monster/AttackActor/GnuLavaBurstCollisionActor.h"
// 위젯
#include "Monster/Widget/GnuMonsterHealthBase.h"
// 라이브러리 함수
#include "Materials/MaterialInstanceDynamic.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/GnuMyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"				// 타이머 사용을 위한 헤더 추가
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "KismetAnimationLibrary.h"
#include <Net/UnrealNetwork.h>
#include "GNU_PBL/GNU_PBL.h"	// SkeletalMesh 채널 설정
// 캐릭터
#include "Characters/GnuMyCharacter.h"
// 게임모드
#include "GameModes/GNUGameMode.h"

AGnuMonster::AGnuMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컨트롤러의 회전 사용 여부
	bUseControllerRotationYaw = false;

	// 기본 캡슐 컴포넌트, 기본 스켈레탈 메쉬수정 가능하도록 설정
	GetCapsuleComponent()->bEditableWhenInherited = true;
	GetMesh()->bEditableWhenInherited = true;

	// 콜리전 초기화
	//InitializeCollisionComponent(ClawCollision, TEXT("ClawCollision"));	// 손톱 공격 콜리전
	//InitializeCollisionComponent(TailCollision, TEXT("TailCollision")); // 꼬리 공격 콜리전

	ClawCollision = CreateDefaultSubobject<UBoxComponent>("ClawCollision");
	ClawCollision->SetupAttachment(GetMesh());
	ClawCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// 기본적으로 비활성화
	ClawCollision->bEditableWhenInherited = true; // 블루프린트 값이 우선되게 함

	TailCollision = CreateDefaultSubobject<UBoxComponent>("TailCollision");
	TailCollision->SetupAttachment(GetMesh());
	TailCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// 기본적으로 비활성화
	TailCollision->bEditableWhenInherited = true; // 블루프린트 값이 우선되게 함

	BodyCollision = CreateDefaultSubobject<USphereComponent>("BodyCollision");
	BodyCollision->SetupAttachment(GetMesh());
	BodyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// 기본적으로 비활성화
	BodyCollision->bEditableWhenInherited = true; // 블루프린트 값이 우선되게 함
	
	// 캐릭간 공격을 막기 위한 새로운 채널 설정
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	// MovementComponent 리플리케이션 활성화
	GetCharacterMovement()->SetIsReplicated(true);
	SetReplicateMovement(true);
	bReplicates = true; // Actor 리플리케이션 활성화
	bAlwaysRelevant = true; // 항상 네트워크에서 중요

	// hp 구현
	MaxHealth = 1000.f;
	CurrentHealth = MaxHealth;

	// 넉백 힘 초기화
	KnockbackStrength = 0.f;
}

void AGnuMonster::BeginPlay()
{
	Super::BeginPlay();
	
	DeactivateSkeletalMesh();	// 타겟 인식 전 콜리전 비활성화
	ActivateCapsuleComp();		// 캡슐 컴포넌트는 활성화

	if (BodyCollision)
	{
		BodyCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnuMonster::OnBodyOverlapBegin);
	}

	if (ClawCollision)
	{
		ClawCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnuMonster::OnClawOverlapBegin);
	}

	if (TailCollision)
	{
		TailCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnuMonster::OnTailOverlapBegin);
	}

	// 서버에서 데미지 관리
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AGnuMonster::ReceiveDamage);
	}
	
	// Dynamic Material Instance 생성
	UMaterialInterface* Material_1st = GetMesh()->GetMaterial(0);  // 첫 번째 머티리얼 인덱스 사용
	UMaterialInterface* Material_2nd = GetMesh()->GetMaterial(1);  // 두 번째 머티리얼 인덱스 사용
	if (Material_1st && Material_2nd)
	{
		DynamicMaterialInst_1st = UMaterialInstanceDynamic::Create(Material_1st, this);
		GetMesh()->SetMaterial(0, DynamicMaterialInst_1st);

		DynamicMaterialInst_2nd = UMaterialInstanceDynamic::Create(Material_2nd, this);
		GetMesh()->SetMaterial(1, DynamicMaterialInst_2nd);
	}
}


void AGnuMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		SetGroundSpeed();
		SetDirection();
	}

	// FirebreathActor가 활성화 되면 위치 업데이트
	if (FirebreathActor)
	{

		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("HeadSocket")) + FVector(0.f, 0.f, -50.f);
		FVector SpawnLocation = HeadLocation + GetActorForwardVector();  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetMesh()->GetSocketRotation(TEXT("HeadSocket"));
		SpawnRotation.Pitch += 70;

		FirebreathActor->SetActorLocation(SpawnLocation + SpawnRotation.Vector() * 100); // 몬스터 앞쪽으로 이동
		FirebreathActor->SetActorRotation(SpawnRotation); // 몬스터와 같은 방향으로 회전

		if (bCanRetry)
		{
			FirebreathActor->LaunchProjectile(this, &SpawnLocation, &SpawnRotation);

			// 재시도 대기 시간 설정
			StartRetryCooldown();
		}
	}
}

void AGnuMonster::SetGroundSpeed()
{
	GroundSpeed = GetVelocity().Size();	// 몬스터 이속 업데이트
}

void AGnuMonster::SetHealthWidget_Implementation()
{
	// Health 위젯이 설정되어 있으면 생성하여 화면에 추가
	// 플레이어를 인식하면 UI가 뜨도록 변경해야함 (추후 수정)
	if (MonsterHealthWidgetClass)
	{
		MonsterHealthWidget = CreateWidget<UGnuMonsterHealthBase>(GetWorld(), MonsterHealthWidgetClass);
		MonsterHealthWidget->AddToViewport();
	}
}

void AGnuMonster::SetDirection()
{
	// 방향 업데이트
	if (AGnuMonsterAIController* AIController = Cast<AGnuMonsterAIController>(GetController()))
	{
		if (AIController->TargetActor != nullptr)
		{
			FVector MonsterForward = GetActorForwardVector();
			FVector ToTarget = (AIController->TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			// 몬스터의 정면 방향과 타겟 방향 간의 각도 차이
			FVector CrossProduct = FVector::CrossProduct(MonsterForward, ToTarget);
			float DotProduct = FVector::DotProduct(MonsterForward, ToTarget);
			Direction = FMath::RadiansToDegrees(FMath::Atan2(CrossProduct.Z, DotProduct));
		}
		else
		{
			Direction = 0.f; // 타겟이 없으면 방향 초기화
		}
	}
}


//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Damage 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ //
// HP 업데이트 함수(멀티 처리)
void AGnuMonster::OnRep_Health()
{
	if (MonsterHealthWidget)
	{
		MonsterHealthWidget->UpdateBossHP(CurrentHealth, MaxHealth);
	}
}

// 데미지를 받게 되었을 때
void AGnuMonster::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	// 머티리얼의 이미시브 색상 변경
	if (DynamicMaterialInst_1st && DynamicMaterialInst_2nd)
	{
		// 머티리얼의 첫번째 두번째 인덱스 찾아서 EmissiveColor 라는 이름을 찾으면 그 값을 변경
		// VectorParam을 이용해서 변경
		Multicast_SetEmissiveColor(FLinearColor(0.05f, 0.0f, 0.0f));


		// 타이머로 원래 색상으로 복원
		FTimerHandle TimerHandle_1st;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_1st, [this]() {
			if (DynamicMaterialInst_1st && DynamicMaterialInst_2nd)
			{
				Multicast_SetEmissiveColor(FLinearColor(0.0f, 0.0f, 0.0f));  // 기본값 (검정색)
			}
			}, 0.05f, false);
	}

	if (MonsterHealthWidget)
	{
		MonsterHealthWidget->UpdateBossHP(CurrentHealth, MaxHealth); 			
	}

	if (AGnuMonsterAIController* AIController = Cast<AGnuMonsterAIController>(GetController()))
	{
		AIController->UpdateMonsterHealth();
	}

	// 몬스터 죽음 처리
	if (CurrentHealth <= 0)
	{
		Die();
		bIsDead = true;
		
		// 죽자마자 바로 결과 화면이 나오기 때문에 타이머를 이용해도 괜찮을듯
		AGNUGameMode* GNUGameMode = GetWorld()->GetAuthGameMode<AGNUGameMode>();
		if (GNUGameMode)
		{
			GNUGameMode->MonsterEliminated();
		}
	}
}

void AGnuMonster::Multicast_SetEmissiveColor_Implementation(const FLinearColor& NewColor)
{
	if (DynamicMaterialInst_1st && DynamicMaterialInst_2nd)
	{
		DynamicMaterialInst_1st->SetVectorParameterValue(FName("EmissiveColor"), NewColor);
		DynamicMaterialInst_2nd->SetVectorParameterValue(FName("EmissiveColor"), NewColor);
	}
}

void AGnuMonster::KnockbackPlayer(ACharacter* PlayerCharacter)
{
	if (PlayerCharacter)
    {
        // 넉백 방향 계산
        FVector KnockbackDirection = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal(); // 적으로부터 플레이어 방향

        // 캐릭터를 뒤로 밀어내기
        PlayerCharacter->LaunchCharacter(KnockbackDirection * KnockbackStrength, true, true);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is nullptr"));
    }
}

void AGnuMonster::StartRetryCooldown()
{
	bCanRetry = false; // 재시도 불가 상태로 변경

	// 일정 시간이 지난 후 재시도 가능 상태로 변경
	GetWorld()->GetTimerManager().SetTimer(
		RetryCooldownTimerHandle,
		[this]() { bCanRetry = true;},
		0.1f, // 쿨다운 시간 (초)
		false // 반복하지 않음
	);
}

void AGnuMonster::Die()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, TEXT("Boss has died!"));

	// FirebreathActor 제거
	if (FirebreathActor)
	{
		FirebreathActor->Destroy();
		FirebreathActor = nullptr; // 포인터 초기화
	}

	// AIController에서 Behavior Tree 중지
	AGnuMonsterAIController* AIController = Cast<AGnuMonsterAIController>(GetController());
	if (AIController)
	{
		AIController->StopBehaviorTree();
	}

	// 몬스터 죽었을 때 메시의 충돌 비활성화
	DeactivateSkeletalMesh();

	// 몬스터 죽었을 때 캡슐 컴포넌트 충돌 비활성화
	DeactivateCapsuleComp();

	
	UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(this->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{	
		// 죽음 상태 설정
		AnimInstance->bIsDead = true;
		AnimInstance->PlayDieMontage();

		// 몽타주가 종료되었는지 확인 후 Destroy 호출
		if (AnimInstance->bIsDead)
		{
			// 12초 타이머 설정
			GetWorld()->GetTimerManager().SetTimer(
				DestroyTimerHandle,						// 타이머 핸들
				this,									// 호출 객체
				&AGnuMonster::DelayedDestroy,			// 호출할 함수
				12.0f,									// 지연 시간 (초)
				false									// 반복 여부 (false : 한 번만 실행)
			);
		}
	}
}

// 몬스터가 죽었을 때 딜레이 주고 삭제시키기
void AGnuMonster::DelayedDestroy()
{
	Destroy();
}

void AGnuMonster::EnterPhaseTwo()
{
	UE_LOG(LogTemp, Warning, TEXT("Entering Phase Two!"));
}
//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Damage 관련 끝 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ //



//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Attack 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ //
// 원거리 공격 부분 5개
void AGnuMonster::SpawnFireball()
{
	if (FireballClass)  // FireballClass는 BP에서 설정한 파이어볼 클래스
	{
		FVector HeadLoaction = GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		FVector SpawnLocation = HeadLoaction + GetActorForwardVector() * 100;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                         // Fireball의 소유자를 현재 몬스터로 설정
		SpawnParams.Instigator = Cast<APawn>(this);       // Instigator(데미지를 유발한 주체)를 현재 몬스터로 설정
		SpawnParams.SpawnCollisionHandlingOverride =							// 생성된 액터가 스폰될 때 충돌 처리 방식 설정 
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	// 충돌이 발생할 경우, 가능한 위치로 조정하여 액터를 생성


		// 파이어볼 액터를 스폰
		AGnuFireballActor* Fireball = GetWorld()->SpawnActor<AGnuFireballActor>(FireballClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Fireball)
		{
			// 파이어볼을 발사하는 메서드 호출
			Fireball->LaunchProjectile(this);
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, TEXT("Start Fireball LaunchProjectile"));*/
		}
	}
}

void AGnuMonster::SpawnFiretornado()
{
	if (FiretornadoClass)
	{
		// 컨트롤러 회전값 사용하여 방향 계산
		FRotator MonsterRotation = GetActorRotation();					// 몬스터의 컨트롤러 회전값
		FVector ForwardVector = MonsterRotation.Vector();				// 몬스터의 정면 방향
		FVector RightVector = FVector::CrossProduct(ForwardVector, FVector::UpVector); // 몬스터의 오른쪽 방향

		// 기본 위치: 몬스터 정면 500 유닛 거리
		FVector BaseSpawnLocation = GetActorLocation() + ForwardVector * 500.f;
		BaseSpawnLocation.Z = 0.f; // 바닥 높이 고정

		// 좌우 위치 계산
		FVector SpawnLocation1 = BaseSpawnLocation;                     // 정면
		FVector SpawnLocation2 = BaseSpawnLocation + RightVector * 500; // 오른쪽
		FVector SpawnLocation3 = BaseSpawnLocation - RightVector * 500; // 왼쪽

		FRotator SpawnRotation1 = MonsterRotation;
		FRotator SpawnRotation2 = MonsterRotation;
		SpawnRotation2.Yaw -= 30;
		FRotator SpawnRotation3 = MonsterRotation;
		SpawnRotation3.Yaw += 30;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                         // 소유자를 현재 몬스터로 설정
		SpawnParams.Instigator = Cast<APawn>(this);       // Instigator(데미지를 유발한 주체)를 현재 몬스터로 설정
		SpawnParams.SpawnCollisionHandlingOverride =							// 생성된 액터가 스폰될 때 충돌 처리 방식 설정 
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	// 충돌이 발생할 경우, 가능한 위치로 조정하여 액터를 생성

		
		if (HasAuthority())
		{
			AGnuFiretornadoActor* Firetornado1 = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation1, SpawnRotation1, SpawnParams);
			AGnuFiretornadoActor* Firetornado2 = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation2, SpawnRotation2, SpawnParams);
			AGnuFiretornadoActor* Firetornado3 = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation3, SpawnRotation3, SpawnParams);

			if (Firetornado1 && Firetornado2 && Firetornado3)
			{
				Firetornado1->LaunchProjectile(this);
				Firetornado2->LaunchProjectile(this);
				Firetornado3->LaunchProjectile(this);
			}
		}
		
	}
}

void AGnuMonster::SpawnFirebreath()
{
	if (FirebreathClass)
	{
		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		FVector SpawnLocation = HeadLocation + GetActorForwardVector() * 50;  // 몬스터 앞에 생성
		FRotator HeadRotation = GetMesh()->GetSocketRotation(TEXT("HeadSocket"));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                         // 소유자를 현재 몬스터로 설정
		SpawnParams.Instigator = Cast<APawn>(this);       // Instigator(데미지를 유발한 주체)를 현재 몬스터로 설정
		SpawnParams.SpawnCollisionHandlingOverride =							// 생성된 액터가 스폰될 때 충돌 처리 방식 설정 
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	// 충돌이 발생할 경우, 가능한 위치로 조정하여 액터를 생성

		if (FirebreathActor == nullptr)
		{
			FirebreathActor = GetWorld()->SpawnActor<AGnuFirebreathActor>(FirebreathClass, SpawnLocation, HeadRotation, SpawnParams);
		}
		else
		{
			return;
		}
	}
}

void AGnuMonster::SpawnGroundAttack()
{
	if (GroundClass)
	{
		FVector GroundLoaction = GetMesh()->GetSocketLocation(TEXT("GroundSocket"));
		GroundLoaction.Z = 0.f;

		FVector SpawnLocation = GroundLoaction + GetActorForwardVector() * 600;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                         // Fireball의 소유자를 현재 몬스터로 설정
		SpawnParams.Instigator = Cast<APawn>(this);       // Instigator(데미지를 유발한 주체)를 현재 몬스터로 설정
		SpawnParams.SpawnCollisionHandlingOverride =							// 생성된 액터가 스폰될 때 충돌 처리 방식 설정 
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	// 충돌이 발생할 경우, 가능한 위치로 조정하여 액터를 생성

		// 그라운드 액터를 스폰
		AGnuGroundActor* Ground = GetWorld()->SpawnActor<AGnuGroundActor>(GroundClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Ground)
		{
			// Ground Collision Box Component를 발사하는 메서드 호출
			Ground->LaunchProjectile(this, &SpawnLocation, &SpawnRotation);
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, TEXT("Start Ground LaunchProjectile"));*/
		}
	}
}

void AGnuMonster::SpawnGroundSpikeAttack()
{
	if (GroundSpikeClass && GroundSpikeCollisionClass)
	{
		FVector NiagaraSpawnLocation = GetActorLocation();
		NiagaraSpawnLocation.Z = 0.f;
		FRotator NiagaraSpawnRotation = GetActorRotation();

		// 그라운드 스파이크 액터를 스폰 (나이아가라 시스템 스폰)
		AGnuGroundSpikeActor* GroundSpike = GetWorld()->SpawnActor<AGnuGroundSpikeActor>(GroundSpikeClass, NiagaraSpawnLocation, NiagaraSpawnRotation);

		FVector CenterLocation = GetActorLocation(); // 중심 위치
		const float Radius = 700.f; // 몬스터와 GroundSpikeCollisionActor 사이의 거리
		const int32 SpikeCount = 5; // 스파이크 개수
		const float AngleIncrement = 360.f / SpikeCount; // 각 스파이크 간의 각도 간격

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                         // 소유자를 현재 몬스터로 설정
		SpawnParams.Instigator = Cast<APawn>(this);       // Instigator(데미지를 유발한 주체)를 현재 몬스터로 설정
		SpawnParams.SpawnCollisionHandlingOverride =							// 생성된 액터가 스폰될 때 충돌 처리 방식 설정 
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	// 충돌이 발생할 경우, 가능한 위치로 조정하여 액터를 생성


		// 나이아가라 파티클 방향에 맞는 콜리전 스폰 (5방향, 5개)
		for (int32 i = 0; i < SpikeCount; i++)
		{
			float CurrentAngle = i * AngleIncrement;	// 각도 계산

			// 방향 벡터를 계산
			FRotator SpawnRotation = GetActorRotation();
			SpawnRotation.Yaw += CurrentAngle; // 중심에서 Yaw 값을 증가
			FVector SpawnDirection = SpawnRotation.Vector();

			// 위치 계산: 중심에서 SpawnDirection 방향으로 Radius 만큼 이동
			FVector SpawnLocation = CenterLocation + SpawnDirection * Radius;

			// GroundSpikeCollisionActor 생성
			AGnuGroundSpikeCollisionActor* GroundSpikeCollision = 
				GetWorld()->SpawnActor<AGnuGroundSpikeCollisionActor>(GroundSpikeCollisionClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (GroundSpikeCollision)
			{
				// GroundSpikeCollisionActor 발사
				GroundSpikeCollision->LaunchProjectile(this);
			}
		}
	}
}

// 체력 반피 이하일때 HP 회복 10% / 패턴 발동
void AGnuMonster::StartCraterAttack()
{
	// 타이머 초기화
	ScheduleNextCrater(CraterTimerHandle1);
	ScheduleNextCrater(CraterTimerHandle2);
	ScheduleNextCrater(CraterTimerHandle3);
	ScheduleNextCrater(CraterTimerHandle4);
	ScheduleNextCrater(CraterTimerHandle5);

	DeactivateCapsuleComp();
}

void AGnuMonster::EndCraterAttack()
{
	GetWorldTimerManager().ClearTimer(CraterTimerHandle1);
	GetWorldTimerManager().ClearTimer(CraterTimerHandle2);
	GetWorldTimerManager().ClearTimer(CraterTimerHandle3);
	GetWorldTimerManager().ClearTimer(CraterTimerHandle4);
	GetWorldTimerManager().ClearTimer(CraterTimerHandle5);

	ActivateCapsuleComp();
}

void AGnuMonster::ScheduleNextCrater(FTimerHandle& TimerHandle)
{
	// 1.5 ~ 2.5 범위의 랜덤 간격 생성
	float RandomInterval = FMath::RandRange(0.5f, 1.5f);

	// 랜덤 간격으로 타이머 설정 (각각 독립적으로 작동)
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AGnuMonster::SpawnCraterAndReschedule, RandomInterval, false);
}

void AGnuMonster::SpawnCraterAndReschedule()
{
	// Crater 스폰
	if (!bIsDead)
	{
		SpawnCrater();
	}
	else
	{
		EndCraterAttack();
	}
	
	// 호출된 타이머 핸들을 통해 다시 스케줄링
	if (CraterTimerHandle1.IsValid())
	{
		ScheduleNextCrater(CraterTimerHandle1);
	}
	else if (CraterTimerHandle2.IsValid())
	{
		ScheduleNextCrater(CraterTimerHandle2);
	}
	else if (CraterTimerHandle3.IsValid())
	{
		ScheduleNextCrater(CraterTimerHandle3);
	}
	else if (CraterTimerHandle4.IsValid())
	{
		ScheduleNextCrater(CraterTimerHandle3);
	}
	else if (CraterTimerHandle5.IsValid())
	{
		ScheduleNextCrater(CraterTimerHandle3);
	}
}

void AGnuMonster::SpawnCrater()
{
	if (!CraterActorClass) return;

	FVector Origin = GetActorLocation();
	Origin.Z = 0.f;

	// 랜덤 방향과 거리
	float RandomAngle = FMath::RandRange(0.f, 360.f); // 0도 ~ 360도
	float RandomDistance = FMath::RandRange(0.f, 4000.f); // 최소 ~ 최대 반경

	// 랜덤한 방향으로 위치 계산
	FVector RandomOffset = FVector(
		FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
		FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
		0.f // 높이는 고정
	);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;                         // 소유자를 현재 몬스터로 설정
	SpawnParams.Instigator = Cast<APawn>(this);       // Instigator(데미지를 유발한 주체)를 현재 몬스터로 설정
	SpawnParams.SpawnCollisionHandlingOverride =							// 생성된 액터가 스폰될 때 충돌 처리 방식 설정 
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	// 충돌이 발생할 경우, 가능한 위치로 조정하여 액터를 생성

	FVector SpawnLocation = Origin + RandomOffset;

	// 소환 (Niagara Comp / Attack Collision)
	AGnuLavaBurstActor* SpawnCrater = GetWorld()->SpawnActor<AGnuLavaBurstActor>(CraterActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	AGnuLavaBurstCollisionActor* SpawnCraterCollision = GetWorld()->SpawnActor<AGnuLavaBurstCollisionActor>(CraterCollisionClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (SpawnCraterCollision)
	{
		SpawnCraterCollision->LaunchProjectile(this); // 발사 실행
	}
}

// 근거리 공격 부분 (3개)
// 시발 왜 Trace가 클라이언트한테만 먹고 서버한테는 안먹을까?
void AGnuMonster::BodyAttack()
{
	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	// 몬스터가 10도 어긋난채로 몸통박치기함 이걸로 해결
	FRotator CurrentRotation = this->GetActorRotation();	// 몬스터의 현재 회전값을 받아옴
	CurrentRotation.Yaw += 10.0f;							// 현재 회전값에 10도 추가 (Yaw 값만 수정)
	this->SetActorRotation(CurrentRotation);				// 수정된 회전값을 애니메이션에 적용
	// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

	//float SphereRadius = 220.0f; // Sphere Trace 반경
	//FVector Start = GetActorLocation();

	//FRotator Rotation = GetActorRotation(); // 몬스터의 현재 회전
	//Rotation.Yaw -= 40.f;                  // Yaw 방향 -40도

	//FVector ForwardDirection = Rotation.Vector(); // 회전된 방향의 벡터를 가져옴

	//FVector End = Start + (ForwardDirection * 1900.0f); // 1800.0 전방 이동
	//FHitResult HitResult;	// 충돌 결과를 담는 변수

	//// Trace 실행
	//bool bHit = GetWorld()->SweepSingleByChannel(
	//	HitResult,
	//	Start,
	//	End,
	//	FQuat::Identity,
	//	ECC_Visibility,
	//	FCollisionShape::MakeSphere(SphereRadius) //  220 반경
	//);

	//// 디버그: Sphere Trace의 경로를 표시
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f); // 시작점과 끝점을 잇는 선
	//DrawDebugSphere(GetWorld(), Start, SphereRadius, 12, FColor::Blue, false, 2.0f); // 시작점에 구체 표시
	//DrawDebugSphere(GetWorld(), End, SphereRadius, 12, FColor::Red, false, 2.0f);   // 끝점에 구체 표시

	//if (bHit && HitResult.GetActor() != this)
	//{
	//	AActor* HitActor = HitResult.GetActor();
	//	FString HitActorName = HitActor->GetName();
	//	GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Orange, FString::Printf(TEXT("Hit! %s"), *HitActorName));
	//	
	//	AGnuMyCharacter* TargetCharacter = Cast<AGnuMyCharacter>(HitResult.GetActor());
	//	if (TargetCharacter)
	//	{
	//		FString HitName = TargetCharacter->GetName();
	//		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, FString::Printf(TEXT("Target Character set %s"), *HitName));
	//		// 디버그: 충돌한 Actor의 위치에 구체 표시
	//		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 20.f, 12, FColor::Yellow, false, 2.0f);
	//	}
	//	else
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, FString::Printf(TEXT("Hit Actor: %s is not TargetCharacter"), *HitActorName));
	//	}
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, FString::Printf(TEXT("Hit Actor is nullptr")));
	//}
}


// Claw 공격시 overlap 되면 불러질 함수
void AGnuMonster::OnClawOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// 캐릭터 확인
		AGnuMyCharacter* TargetCharacter = Cast<AGnuMyCharacter>(OtherActor);
		if (TargetCharacter)
		{
			// 마지막 타격 시간 가져오기
			float* LastHit = LastHitTime.Find(OtherActor);
			float CurrentTime = GetWorld()->GetTimeSeconds();

			// 타격 간격 내에 맞은 캐릭터는 무시하고, 타격을 받은 시간이 충분하면 데미지 적용
			if (LastHit && (CurrentTime - *LastHit) < AttackCooldown)
			{
				return; // 타격이 간격 이내에 발생했으면 처리하지 않음
			}

			// 데미지 처리
			float DamageAmount = 30.0f; // 데미지 양
			UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetController(), this, UDamageType::StaticClass());
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Claw Attack Overlap"));*/

			// 플레이어에게 넉백 적용
			KnockbackStrength = 5000.f;
			KnockbackPlayer(TargetCharacter);

			LastHitTime.Add(OtherActor, CurrentTime);
		}
		else
		{
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("This is not TargetChracter"));*/
		}
	}
}

// Tail 공격시 overlap 되면 불러질 함수
void AGnuMonster::OnTailOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// 캐릭터 확인
		AGnuMyCharacter* TargetCharacter = Cast<AGnuMyCharacter>(OtherActor);
		if (TargetCharacter)
		{
			// 마지막 타격 시간 가져오기
			float* LastHit = LastHitTime.Find(OtherActor);
			float CurrentTime = GetWorld()->GetTimeSeconds();

			// 타격 간격 내에 맞은 캐릭터는 무시하고, 타격을 받은 시간이 충분하면 데미지 적용
			if (LastHit && (CurrentTime - *LastHit) < AttackCooldown)
			{
				return; // 타격이 간격 이내에 발생했으면 처리하지 않음
			}

			// 데미지 처리
			float DamageAmount = 40.0f; // 데미지 양
			UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetController(), this, UDamageType::StaticClass());
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Tail Take Damage Overlap"));*/

			// 플레이어에게 넉백 적용
			KnockbackStrength = 10000.f;
			KnockbackPlayer(TargetCharacter);

			LastHitTime.Add(OtherActor, CurrentTime);
		}
		else
		{
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("This is not TargetChracter"));*/
		}
	}
}

// Body 공격시 overlap 되면 불러질 함수
void AGnuMonster::OnBodyOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// 캐릭터 확인
		AGnuMyCharacter* TargetCharacter = Cast<AGnuMyCharacter>(OtherActor);
		if (TargetCharacter)
		{
			// 마지막 타격 시간 가져오기
			float* LastHit = LastHitTime.Find(OtherActor);
			float CurrentTime = GetWorld()->GetTimeSeconds();

			// 타격 간격 내에 맞은 캐릭터는 무시하고, 타격을 받은 시간이 충분하면 데미지 적용
			if (LastHit && (CurrentTime - *LastHit) < AttackCooldown)
			{
				return;
			}

			// 데미지 처리
			float DamageAmount = 35.0f; // 데미지 양
			UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetController(), this, UDamageType::StaticClass());
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Tail Take Damage Overlap"));*/

			// 플레이어에게 넉백 적용
			KnockbackStrength = 30000.f;
			KnockbackPlayer(TargetCharacter);

			LastHitTime.Add(OtherActor, CurrentTime);
		}
		else
		{
			/*GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("This is not TargetChracter"));*/
		}
	}
}
//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Attack 관련 끝 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Monster Collision 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
void AGnuMonster::ActivateSkeletalMesh()
{
	// 스켈레탈 메시 활성화
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AGnuMonster::ActivateCapsuleComp()
{
	// 캡슐 컴포넌트 활성화
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AGnuMonster::DeactivateSkeletalMesh()
{
	// 스켈레탈 메시 비활성화
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGnuMonster::DeactivateCapsuleComp()
{
	// 캡슐 컴포넌트 비활성화
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// 공격 컴포넌트
void AGnuMonster::ActivateClawCollision()
{
	ClawCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 콜리전 활성화
}

void AGnuMonster::DeactivateClawCollision()
{
	ClawCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 콜리전 비활성화
}

void AGnuMonster::ActivateTailCollision()
{
	TailCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGnuMonster::DeactivateTailCollision()
{
	TailCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGnuMonster::ActivateBodyCollision()
{
	BodyCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGnuMonster::DeactivateBodyCollision()
{
	BodyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Monster Collision 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ


// Claw, Tail 콜리전 초기화로 만들었던 초기설정 함수
void AGnuMonster::InitializeCollisionComponent(UBoxComponent*& CollisionComponent, const FName& ComponentName)
{
	//CollisionComponent = CreateDefaultSubobject<UBoxComponent>(ComponentName);
	//CollisionComponent->SetupAttachment(GetMesh());
	//CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 오버랩을 위해 QueryOnly로 설정
}

// 블루프린트에서 Attach Socket 설정 가능한 가변형 함수, 사용시 주의해야함
#if WITH_EDITOR
void AGnuMonster::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, ClawCollisionBoxAttachBoneName))
	{
		ClawCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ClawCollisionBoxAttachBoneName);
		ClawCollision->SetWorldScale3D(FVector(0.25f)); // 스케일을 1/4배로 설정 => 기존 스케일이 너무 커서 0.25로 설정해둠
	}
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, TailCollisionBoxAttachBoneName))
	{
		TailCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TailCollisionBoxAttachBoneName);
		TailCollision->SetWorldScale3D(FVector(0.25f)); // 스케일을 1/4배로 설정 => 기존 스케일이 너무 커서 0.25로 설정해둠
	}
}
#endif


// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 멀티 관련 시작 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ //
// 전체적 값 복제 함수
void AGnuMonster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AGnuMonster, CurrentHealth);

	// GroundSpeed를 복제
	DOREPLIFETIME(AGnuMonster, GroundSpeed);

	DOREPLIFETIME(AGnuMonster, Direction);

}

void AGnuMonster::OnRep_GroundSpeed()
{
}

void AGnuMonster::OnRep_Direction()
{
}

void AGnuMonster::MulticastPlayMontage_Implementation(UAnimMontage* MontageToPlay)
{
	UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayMontage(MontageToPlay);
	}
}
// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 멀티 관련 끝 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ //