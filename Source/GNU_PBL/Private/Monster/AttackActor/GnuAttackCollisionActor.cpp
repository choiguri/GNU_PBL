// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "Characters/GnuMyCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AGnuAttackCollisionActor::AGnuAttackCollisionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // 복제 활성화

	// 초기화 and 부모설정
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(GetRootComponent());

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	StaticMesh->SetupAttachment(BoxComponent);

	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	Particle->SetupAttachment(BoxComponent);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(BoxComponent);
	Arrow->SetWorldRotation(FRotator(0.0f, 90.0f, 0.0f));  // Y축으로 90도 회전

	// 파티클의 속성 설정 (필요 시)
	//Particle->bAutoActivate = false; // 자동으로 활성화되지 않도록 설정
	//Particle->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // 위치 설정

	DamageType = UDamageType::StaticClass();
	Damage = 1.0f;
}

void AGnuAttackCollisionActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Overlap with: ") + OtherActor->GetName());
	}
}

void AGnuAttackCollisionActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGnuMyCharacter* Character = Cast<AGnuMyCharacter>(OtherActor);
	if (Character != nullptr)
	{
		Character->SetOverlapItem(this);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("EndOverlap Attacking.."));
			// 공격이 맞았는지 여부 출력
		}
	}
}

// Called when the game starts or when spawned
void AGnuAttackCollisionActor::BeginPlay()
{
	Super::BeginPlay();

	// AddDynamic: 겹쳐졌으면 내가 원하는 함수가 실행되도록 바인딩하는 것
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuAttackCollisionActor::BeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGnuAttackCollisionActor::EndOverlap);
	
	if (Particle)
	{
		Particle->Activate();
	}
}

// Called every frame
void AGnuAttackCollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

