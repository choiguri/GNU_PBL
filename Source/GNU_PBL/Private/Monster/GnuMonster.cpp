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
// 무기 관련
#include "Weapons/Bullet.h"
// 위젯
#include "Monster/Widget/GnuMonsterHealthBase.h"
// 라이브러리 함수
#include "Materials/MaterialInstanceDynamic.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/GnuMyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"				// 타이머 사용을 위한 헤더 추가
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Net/UnrealNetwork.h>
#include "GNU_PBL/GNU_PBL.h"


AGnuMonster::AGnuMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	ClawCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ClawCollision->bEditableWhenInherited = true; // 블루프린트 값이 우선되게 함
	ClawCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnuMonster::OnClawOverlapBegin);

	TailCollision = CreateDefaultSubobject<UBoxComponent>("TailCollision");
	TailCollision->SetupAttachment(GetMesh());
	TailCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TailCollision->bEditableWhenInherited = true; // 블루프린트 값이 우선되게 함
	TailCollision->OnComponentBeginOverlap.AddDynamic(this, &AGnuMonster::OnTailOverlapBegin);

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	// hp 구현
	MaxHealth = 1000.f;
	CurrentHealth = MaxHealth;

	// 넉백 힘
	KnockbackStrength = 0.f;
}


void AGnuMonster::BeginPlay()
{
	Super::BeginPlay();

	// 타겟 인식 전 콜리전 비활성화
	DeactivateSkeletalMesh();
	DeactivateCapsuleComp();

	// Health 위젯이 설정되어 있으면 생성하여 화면에 추가
	// 플레이어를 인식하면 UI가 뜨도록 변경해야함 (추후 수정)
	if (MonsterHealthWidgetClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Call MonsterHealthWidgetClass"));

		MonsterHealthWidget = CreateWidget<UGnuMonsterHealthBase>(GetWorld(), MonsterHealthWidgetClass);
		MonsterHealthWidget->AddToViewport();
	}

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

	// FirebreathActor가 활성화 되면 위치 업데이트
	if (FirebreathActor)
	{
		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("HeadSocket")) + FVector(0.f,0.f, -50.f);
		FVector SpawnLocation = HeadLocation + GetActorForwardVector();  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetMesh()->GetSocketRotation(TEXT("HeadSocket"));
		SpawnRotation.Pitch += 70;

		FirebreathActor->SetActorLocation(SpawnLocation + SpawnRotation.Vector() * 100); // 몬스터 앞쪽으로 이동
		FirebreathActor->SetActorRotation(SpawnRotation); // 몬스터와 같은 방향으로 회전

		if (bCanRetry)
		{
			FirebreathActor->LaunchProjectile(&SpawnLocation, &SpawnRotation);

			// 재시도 대기 시간 설정
			StartRetryCooldown();
		}
	}
}

// HP 업데이트 함수(멀티 처리)
void AGnuMonster::OnRep_Health()
{
	if (MonsterHealthWidget)
	{
		MonsterHealthWidget->UpdateBossHP(CurrentHealth, MaxHealth);
	}
}


void AGnuMonster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AGnuMonster, CurrentHealth);
}


// 데미지를 받게 되었을 때
void AGnuMonster::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, FString::Printf(TEXT("Current Health : %f"), CurrentHealth));
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, FString::Printf(TEXT("Max Health : %f"), MaxHealth));
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	// 머티리얼의 이미시브 색상 변경
	if (DynamicMaterialInst_1st && DynamicMaterialInst_2nd)
	{
		// 머티리얼의 첫번째 두번째 인덱스 찾아서 EmissiveColor 라는 이름을 찾으면 그 값을 변경
		// VectorParam을 이용해서 변경
		DynamicMaterialInst_1st->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor(0.05f, 0.0f, 0.0f));  // 빨간색
		DynamicMaterialInst_2nd->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor(0.05f, 0.0f, 0.0f));  // 빨간색

		// 타이머로 원래 색상으로 복원
		FTimerHandle TimerHandle_1st;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_1st, [this]() {
			if (DynamicMaterialInst_1st)
			{
				DynamicMaterialInst_1st->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor(0.0f, 0.0f, 0.0f));  // 기본값 (검정색)
			}
			}, 0.05f, false);

		FTimerHandle TimerHandle_2nd;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_2nd, [this]() {
			if (DynamicMaterialInst_2nd)
			{
				DynamicMaterialInst_2nd->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor(0.0f, 0.0f, 0.0f));  // 기본값 (검정색)
			}
			}, 0.05f, false);
	}

	if (MonsterHealthWidget)
	{
		MonsterHealthWidget->UpdateBossHP(CurrentHealth, MaxHealth); 			
	}
	if (GEngine)	
	{
		GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Blue, FString::Printf(TEXT("HasAuthority")));

	}

	// 몬스터 죽음 처리
	if (CurrentHealth <= 0)
	{
		Die();
	}
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
			// 데미지 처리
			float DamageAmount = 30.0f; // 데미지 양
			/*TargetCharacter->TakeDamage(DamageAmount, FDamageEvent(), nullptr, this);*/
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Claw Attack Overlap"));

			// 플레이어에게 넉백 적용
			KnockbackStrength = 5000.f;
			KnockbackPlayer(TargetCharacter);
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
			// 데미지 처리
			float DamageAmount = 30.0f; // 데미지 양
			/*TargetCharacter->TakeDamage(DamageAmount, FDamageEvent(), nullptr, this);*/
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Tail Attack Overlap"));

			// 플레이어에게 넉백 적용
			KnockbackStrength = 10000.f;
			KnockbackPlayer(TargetCharacter);
		}
	}
}


void AGnuMonster::KnockbackPlayer(AGnuMyCharacter* PlayerCharacter)
{
	// 넉백 방향 계산
	FVector KnockbackDirection = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal(); // 적으로부터 플레이어 방향

	// 캐릭터를 뒤로 밀어내기
	PlayerCharacter->LaunchCharacter(KnockbackDirection * KnockbackStrength, true, true);
}

void AGnuMonster::StartRetryCooldown()
{
	bCanRetry = false; // 재시도 불가 상태로 변경

	// 일정 시간이 지난 후 재시도 가능 상태로 변경
	GetWorld()->GetTimerManager().SetTimer(
		RetryCooldownTimerHandle,
		[this]() { bCanRetry = true;},
		0.15f, // 쿨다운 시간 (초)
		false // 반복하지 않음
	);
}

void AGnuMonster::Die()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, TEXT("Boss has died!"));

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


//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Collision 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
void AGnuMonster::ActivateSkeletalMesh()
{
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AGnuMonster::ActivateCapsuleComp()
{
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AGnuMonster::DeactivateSkeletalMesh()
{
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGnuMonster::DeactivateCapsuleComp()
{
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Collision 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ


//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Attack 관련 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
void AGnuMonster::SpawnFireball()
{
	if (FireballClass)  // FireballClass는 BP에서 설정한 파이어볼 클래스
	{
		FVector HeadLoaction = GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		FVector SpawnLocation = HeadLoaction + GetActorForwardVector() * 100;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;                         // Fireball의 소유자를 현재 몬스터로 설정
		SpawnParams.Instigator = Cast<APawn>(this);       // Instigator를 현재 몬스터로 설정
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


		// 파이어볼 액터를 스폰
		AGnuFireballActor* Fireball = GetWorld()->SpawnActor<AGnuFireballActor>(FireballClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Fireball)
		{
			// 파이어볼을 발사하는 메서드 호출
			Fireball->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange,TEXT("Start Fireball LaunchProjectile"));
		}
	}
}

void AGnuMonster::SpawnFiretornado()
{
	if (FiretornadoClass)
	{
		FVector BaseSpawnLocation = GetActorLocation() + GetActorForwardVector() * 500;
		BaseSpawnLocation.Z = 0.f;	// 높이 0으로 만들어서 바닥 높이에서 생성되도록

		FVector SpawnLocation1 = BaseSpawnLocation; // 원래 위치
		FVector SpawnLocation2 = BaseSpawnLocation + FVector(0.f, 500.f, 0.f);  // 약간 오른쪽으로 이동
		FVector SpawnLocation3 = BaseSpawnLocation + FVector(0.f, -500.f, 0.f);  // 약간 왼쪽으로 이동


		FRotator SpawnRotation1 = GetActorRotation();
		
		FRotator SpawnRotation2 = GetActorRotation();
		SpawnRotation2.Yaw -= 30;

		FRotator SpawnRotation3 = GetActorRotation();
		SpawnRotation3.Yaw += 30;


		AGnuFiretornadoActor* Firetornado = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation1, SpawnRotation1);
		AGnuFiretornadoActor* Firetornado2 = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation2, SpawnRotation2);
		AGnuFiretornadoActor* Firetornado3 = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation3, SpawnRotation3);

		if (Firetornado && Firetornado2 && Firetornado3)
		{
			Firetornado->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("Start Firetornado LaunchProjectile"));

			Firetornado2->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("Start Firetornado2 LaunchProjectile"));

			Firetornado3->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("Start Firetornado3 LaunchProjectile"));
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

		/*AGnuFirebreathActor* Firebreath = GetWorld()->SpawnActor<AGnuFirebreathActor>(FirebreathClass, SpawnLocation, SpawnRotation);*/
		FirebreathActor = GetWorld()->SpawnActor<AGnuFirebreathActor>(FirebreathClass, SpawnLocation, HeadRotation);
	}
}

void AGnuMonster::SpawnGroundAttack()
{
	if (GroundClass)
	{
		FVector GroundLoaction = GetMesh()->GetSocketLocation(TEXT("GroundSocket"));
		GroundLoaction.Z = 0.f;

		FVector SpawnLocation = GroundLoaction + GetActorForwardVector() * 100;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetActorRotation();

		// 그라운드 액터를 스폰
		AGnuGroundActor* Ground = GetWorld()->SpawnActor<AGnuGroundActor>(GroundClass, SpawnLocation, SpawnRotation);
	}
}

void AGnuMonster::SpawnGroundSpikeAttack()
{
	if (GroundSpikeClass && GroundSpikeCollisionClass)
	{
		FVector NiagaraSpawnLocation = GetActorLocation();
		NiagaraSpawnLocation.Z = 0.f;
		FRotator NiagaraSpawnRotation = GetActorRotation();

		// 그라운드 스파이크 액터를 스폰
		AGnuGroundSpikeActor* GroundSpike = GetWorld()->SpawnActor<AGnuGroundSpikeActor>(GroundSpikeClass, NiagaraSpawnLocation, NiagaraSpawnRotation);

		FVector CenterLocation = GetActorLocation(); // 중심 위치
		const float Radius = 700.f; // 몬스터와 GroundSpikeCollisionActor 사이의 거리
		const int32 SpikeCount = 5; // 스파이크 개수
		const float AngleIncrement = 360.f / SpikeCount; // 각 스파이크 간의 각도 간격

		for (int32 i = 0; i < SpikeCount; i++)
		{
			// 각도를 계산
			float CurrentAngle = i * AngleIncrement;

			// 방향 벡터를 계산
			FRotator SpawnRotation = GetActorRotation();
			SpawnRotation.Yaw += CurrentAngle; // 중심에서 Yaw 값을 증가
			FVector SpawnDirection = SpawnRotation.Vector();

			// 위치 계산: 중심에서 SpawnDirection 방향으로 Radius 만큼 이동
			FVector SpawnLocation = CenterLocation + SpawnDirection * Radius;

			// GroundSpikeCollisionActor 생성
			AGnuGroundSpikeCollisionActor* GroundSpikeCollision = GetWorld()->SpawnActor<AGnuGroundSpikeCollisionActor>(GroundSpikeCollisionClass, SpawnLocation, SpawnRotation);
			if (GroundSpikeCollision)
			{
				// GroundSpikeCollisionActor 발사
				GroundSpikeCollision->LaunchProjectile(this);
			}
		}
	}
}

void AGnuMonster::BodyAttack()
{
	float SphereRadius = 220.0f; // Sphere Trace 반경
	FVector Start = GetActorLocation();

	FRotator Rotation = GetActorRotation(); // 몬스터의 현재 회전
	Rotation.Yaw -= 40.f;                  // Yaw 방향 -40도

	FVector ForwardDirection = Rotation.Vector(); // 회전된 방향의 벡터를 가져옴

	FVector End = Start + (ForwardDirection * 1800.0f); // 1800.0 전방 이동
	FHitResult HitResult;

	// Trace 실행
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SphereRadius) // 220cm 반경
	);

	// 디버그: Sphere Trace의 경로를 표시
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 2.0f); // 시작점과 끝점을 잇는 선
	DrawDebugSphere(GetWorld(), Start, SphereRadius, 12, FColor::Blue, false, 2.0f); // 시작점에 구체 표시
	DrawDebugSphere(GetWorld(), End, SphereRadius, 12, FColor::Red, false, 2.0f);   // 끝점에 구체 표시

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor != this)
		{
			// 데미지 적용
			// UGameplayStatics::ApplyDamage(HitActor, DamageAmount, GetController(), this, DamageTypeClass);

			// 디버그: 충돌한 Actor의 위치에 구체 표시
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, SphereRadius, 12, FColor::Yellow, false, 2.0f);
		}
	}
}

//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ Attack 관련 끝 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ


void AGnuMonster::InitializeAnimInstance()
{
}

void AGnuMonster::InitializeCollisionComponent(UBoxComponent*& CollisionComponent, const FName& ComponentName)
{
	//CollisionComponent = CreateDefaultSubobject<UBoxComponent>(ComponentName);
	//CollisionComponent->SetupAttachment(GetMesh());
	//CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 오버랩을 위해 QueryOnly로 설정
}

// 블루프린트에서 Attach Socket 설정 가능한 가변형 함수 사용시 주의해야함
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



// 공격 콜리전 활성화 부분
void AGnuMonster::ActivateClawCollision()
{
	ClawCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 콜리전 활성화
}

void AGnuMonster::DeactivateClawCollision()
{
	ClawCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 콜리전 비활성화
}

void AGnuMonster::ActivateTailCollision()
{
	TailCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGnuMonster::DeactivateTailCollision()
{
	TailCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
