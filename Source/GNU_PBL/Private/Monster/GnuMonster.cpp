// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"
#include "Monster/GnuMonsterAIController.h"
// 보스 공격 관련
#include "Monster/AttackActor/GnuFireballActor.h"
#include "Monster/AttackActor/GnuFiretornadoActor.h"
#include "Monster/AttackActor/GnuFirebreathActor.h"
#include "Monster/AttackActor/GnuGroundActor.h"
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
#include <Net/UnrealNetwork.h>


AGnuMonster::AGnuMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 컨트롤러의 회전 사용 여부
	bUseControllerRotationYaw = false;

	// 기본 캡슐 컴포넌트, 기본 스켈레탈 메쉬수정 가능하도록 설정
	GetCapsuleComponent()->bEditableWhenInherited = true;
	GetMesh()->bEditableWhenInherited = true;

	// hp 구현
	MaxHealth = 1000.f;
	CurrentHealth = MaxHealth;

	// 넉백 힘
	KnockbackStrength = 0.f;

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
}

void AGnuMonster::BeginPlay()
{
	Super::BeginPlay();

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

	// FirebreathActor의 위치 업데이트
	if (FirebreathActor)
	{
		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		FVector SpawnLocation = HeadLocation + GetActorForwardVector() * 50;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetMesh()->GetSocketRotation(TEXT("HeadSocket"));
		SpawnRotation.Pitch += 70;

		FirebreathActor->SetActorLocation(SpawnLocation + SpawnRotation.Vector() * 100); // 몬스터 앞쪽으로 이동
		FirebreathActor->SetActorRotation(SpawnRotation); // 몬스터와 같은 방향으로 회전
	}
}


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
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 몬스터 죽었을 때 캡슐 컴포넌트 충돌 비활성화
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	
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

void AGnuMonster::DelayedDestroy()
{
	Destroy();
}


void AGnuMonster::EnterPhaseTwo()
{
	UE_LOG(LogTemp, Warning, TEXT("Entering Phase Two!"));
}

void AGnuMonster::SpawnFireball()
{
	if (FireballClass)  // FireballClass는 BP에서 설정한 파이어볼 클래스
	{
		FVector HeadLoaction = GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		FVector SpawnLocation = HeadLoaction + GetActorForwardVector() * 100;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetActorRotation();

		// 파이어볼 액터를 스폰
		AGnuFireballActor* Fireball = GetWorld()->SpawnActor<AGnuFireballActor>(FireballClass, SpawnLocation, SpawnRotation);
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
		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("HeadSocket")) + FVector(0.0f, 0.0f, -80.f);
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

		// 파이어볼 액터를 스폰
		AGnuGroundActor* Ground = GetWorld()->SpawnActor<AGnuGroundActor>(GroundClass, SpawnLocation, SpawnRotation);
	}
}

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
