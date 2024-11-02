// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Bullet.h"
#include "Kismet/GameplayStatics.h"
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	bodyMeshComp->SetEnableGravity(false);
	bodyMeshComp->SetupAttachment(Root);

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	movementComp->SetUpdatedComponent(collisionComp);
	movementComp->InitialSpeed = 10000;
	movementComp->MaxSpeed = 10000;
	movementComp->bShouldBounce = false;

	InitialLifeSpan = 2.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle deathTimer;
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UGameplayStatics::SpawnEmitterAttached(BulletTrail, bodyMeshComp, TEXT("TrailSocket"));

}

void ABullet::Die()
{
	Destroy();
}

void ABullet::SetPower(int power)
{
	Power = power;
}
