// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Gun.h"
#include "Weapons/DamageTest.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include <Characters/GnuCharacter.h>

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

		//Recoil
		float RecoilOffset = GetRecoilOffset(); // Accuracy + MovementStability;
		//Rotation.Pitch += BulletRecoil.Y;
		Rotation.Pitch = FMath::RandRange(Rotation.Pitch, Rotation.Pitch + RecoilOffset);
		//Rotation.Yaw += BulletRecoil.Z;
		Rotation.Yaw = FMath::RandRange(Rotation.Yaw - RecoilOffset, Rotation.Yaw + RecoilOffset);
		//Rotation.Roll += BulletRecoil.X;

		FVector End = Location + Rotation.Vector() * MaxRange;
		// TODO: LineTrace 

		FHitResult Hit;
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);
		if (bSuccess)
		{
			FVector ShotDirection = -Rotation.Vector();
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				// ���Ϳ� Ư�� �±װ� �ִ��� Ȯ�� (��: "WeaponSwitch")
				if (HitActor->ActorHasTag(FName("Enemy")))
				{
					ADamageTest* Enemy = Cast<ADamageTest>(HitActor);
					if (Enemy)
					{
						UE_LOG(LogTemp, Warning, TEXT("Hit Enemy!"));
						Enemy->GetDamage(Power);
					}
				}
			}
		}

		RemainAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("Remaining Ammo: %d"), RemainAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of ammo!"));
		ReleaseTrigger();
	}
	UpdateAmmoDisplay();
}

float AGun::GetRecoilOffset()
{
	AGnuCharacter* GnuCharacter = Cast<AGnuCharacter>(GetOwner());
	if (!IsValid(GnuCharacter)) return 0.f;

	float CharacterSpeed = GnuCharacter->GetVelocity().Size();
	UCharacterMovementComponent* MovementComponent = GnuCharacter->GetCharacterMovement();
	return Accuracy; //* (CurrentRecoilRecoveryTime / RecoilRecoveryTime);// +MovementStability * (CharacterSpeed / MovementComponent->MaxWalkSpeed);
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
	UpdateAmmoDisplay();
}

void AGun::UpdateAmmoDisplay()
{
	if (AmmoDisplay)
	{
		AmmoDisplay->UpdateAmmo(RemainAmmo, MaxAmmo);
	}
}

void AGun::RemoveAmmoDisplay()
{
	if (AmmoDisplay)
	{
		AmmoDisplay->RemoveFromParent();  // UI���� ����
		AmmoDisplay = nullptr;  // ������ �ʱ�ȭ
	}
}



// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	if (AmmoDisplayClass) // ź�� ǥ�� UI ��ȿ�� �˻�
	{
		AmmoDisplay = CreateWidget<UAmmoDisplay>(GetWorld(), AmmoDisplayClass);

		if (AmmoDisplay)
		{
			AmmoDisplay->AddToViewport(); // ����Ʈ �߰�
		}
	}
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

