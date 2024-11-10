// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Bullet.h"
// 데미지 관련
#include "Monster/GnuMonster.h"
#include "GameFramework/DamageType.h"  // FPointDamageEvent를 사용하기 위해 포함
#include "Engine/EngineTypes.h" // FHitResult에 필요한 헤더
#include "Engine/DamageEvents.h"

#include "Kismet/GameplayStatics.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Particles/ParticleSystemComponent.h"
#include <Weapons/DamageTest.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComp->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	CollisionComp->SetupAttachment(Root);

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticle"));
	TrailParticle->SetupAttachment(Root);
	
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	movementComp->InitialSpeed = 20000;
	movementComp->bShouldBounce = false;
	movementComp->ProjectileGravityScale = 0.0f;
	movementComp->SetUpdatedComponent(CollisionComp);

	InitialLifeSpan = 2.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle deathTimer;
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false);

	FVector LaunchDirection = GetActorForwardVector(); // 앞 방향으로 발사
	movementComp->Velocity = LaunchDirection * movementComp->InitialSpeed;

	if (TrailParticleSystem)
	{
		TrailParticle = UGameplayStatics::SpawnEmitterAttached(TrailParticleSystem, CollisionComp);
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit!"));

		// 1. 충돌한 액터의 태그를 확인
		if (ImpactParticle) // 파티클이 설정된 경우
		{
			// 충돌 지점에 파티클 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, Hit.ImpactPoint, FRotator::ZeroRotator);
		}

		if (AGnuMonster* Monster = Cast<AGnuMonster>(OtherActor))
		{
			// FPointDamageEvent를 사용하여 데미지 이벤트 생성
			FPointDamageEvent DamageEvent(Power, Hit, NormalImpulse, UDamageType::StaticClass());
			Monster->TakeDamage(Power, DamageEvent, nullptr, this);
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, FString::Printf(TEXT("Monster hit by projectile, Damage")));
		}
		Destroy();
	}
}


void ABullet::Die()
{
	Destroy();
}

void ABullet::SetPower(float DamageAmount)
{
	Power = DamageAmount;
}

void ABullet::SetDirection(const FVector& Direction)
{
	if (movementComp)
	{
		// 입력된 방향을 기준으로 초기 속도 설정
		movementComp->Velocity = Direction * movementComp->InitialSpeed;
		movementComp->Activate();  // 이동 시작

		FRotator Rotation = Direction.Rotation();
		SetActorRotation(Rotation);
	}
}