// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Actor/GnuActorCollisionBase.h"

// Sets default values
AGnuActorCollisionBase::AGnuActorCollisionBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(GetRootComponent());

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMesh->SetupAttachment(BoxComponent);

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleComponent->SetupAttachment(BoxComponent);


	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(BoxComponent);
	ArrowComponent->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f));  // Y축으로 90도 회전		

}

// Called when the game starts or when spawned
void AGnuActorCollisionBase::BeginPlay()
{
	Super::BeginPlay();

	// AddDynamic: 겹쳐졌으면 내가 원하는 함수가 실행되도록 바인딩하는 것												
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuActorCollisionBase::BeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AGnuActorCollisionBase::EndOverlap);

	if (ParticleComponent)
	{
		ParticleComponent->Activate();
	}
}

void AGnuActorCollisionBase::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGnuActorCollisionBase::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AGnuActorCollisionBase::Tick(float DeltaTime)
{
}
