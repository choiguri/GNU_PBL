// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

}

void AGun::PullTrigger()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AGun::Fire, 60.0 / RPM, true, 0.f);
	}
}

// Trigger release logic (stop firing)
void AGun::ReleaseTrigger()
{
	// Stop the firing timer when the trigger is released
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}


void AGun::Fire()
{
	if (RemainAmmo > 0)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));

		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (OwnerPawn == nullptr) return;
		AController* OwnerController = OwnerPawn->GetController();
		if (OwnerController == nullptr) return;

		FVector Location;
		FRotator Rotation;
		OwnerController->GetPlayerViewPoint(Location, Rotation);

		FVector End = Location + Rotation.Vector() * MaxRange;
		// TODO: LineTrace 

		FHitResult Hit;
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);
		if (bSuccess)
		{
			FVector ShotDirection = -Rotation.Vector();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		}

		RemainAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("Remaining Ammo: %d"), RemainAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of ammo!"));
		ReleaseTrigger();
	}
}


void AGun::Reload()
{
	if (RemainAmmo < MaxAmmo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reloading..."));
		RemainAmmo = MaxAmmo; 
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Already full of ammo"));
	}
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
	RemainAmmo = MaxAmmo;
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

