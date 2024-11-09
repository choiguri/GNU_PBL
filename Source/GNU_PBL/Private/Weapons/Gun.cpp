// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Gun.h"
#include "Weapons/DamageTest.h"
#include "Weapons/Bullet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h" // class UAnimMontage
#include "Net/UnrealNetwork.h" // ~_Implementation(), ~_Validate(), GetLifetimeReplicatedProps()
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include <Characters/GnuMyCharacter.h>

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);


	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBP(TEXT("/Script/Engine.AnimBlueprint'/Game/GNU/Weapons/Rifle/Animation/ABP_Weap_Rifle.ABP_Weap_Rifle'"));
	if (AnimBP.Succeeded())
	{
		Mesh->SetAnimInstanceClass(AnimBP.Object->GeneratedClass);
	}
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
		if (1) //Projectile
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));

			ServerMontageOnFire();

			FTransform fireposition = Mesh->GetSocketTransform(TEXT("FirePosition"), RTS_World);
			FVector spawnLocation = fireposition.GetLocation();

			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			if (OwnerPawn == nullptr) return;
			AController* OwnerController = OwnerPawn->GetController();
			if (OwnerController == nullptr) return;

			FVector Location;
			FRotator Rotation;
			OwnerController->GetPlayerViewPoint(Location, Rotation);

			FVector traceEnd = Location + (Rotation.Vector() * 10000.0f);
			FHitResult hitResult;
			FCollisionQueryParams traceParams(FName(TEXT("ProjectileTrace")), true, this);
			traceParams.bTraceComplex = true;
			traceParams.bReturnPhysicalMaterial = false;

			// 라인트레이스 실행
			bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, Location, traceEnd, ECC_Visibility, traceParams);

			// 히트 판정이 발생한 경우 해당 지점으로 투사체 발사 방향 설정
			FVector targetLocation = bHit ? hitResult.Location : traceEnd;
			FVector shootDirection = (targetLocation - spawnLocation).GetSafeNormal();

			//Recoil
			float RecoilOffset = GetRecoilOffset(); // Accuracy + MovementStability;
			FRotator recoilRotation = shootDirection.Rotation();
			recoilRotation.Pitch = FMath::RandRange(recoilRotation.Pitch, recoilRotation.Pitch + RecoilOffset);
			recoilRotation.Yaw = FMath::RandRange(recoilRotation.Yaw - RecoilOffset, recoilRotation.Yaw + RecoilOffset);

			shootDirection = recoilRotation.Vector();


			ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(bulletFactory, fireposition);
			if (Bullet)
			{
				Bullet->SetPower(DamageAmount);
				Bullet->SetDirection(shootDirection);  // 탄환에 발사 방향 설정
			}
		}
		else // hitscan
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
							Enemy->GetDamage(DamageAmount);
						}
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
	AGnuMyCharacter* GnuMyCharacter = Cast<AGnuMyCharacter>(GetOwner());
	if (!IsValid(GnuMyCharacter)) return 0.f;

	float CharacterSpeed = GnuMyCharacter->GetVelocity().Size();
	UCharacterMovementComponent* MovementComponent = GnuMyCharacter->GetCharacterMovement();
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

void AGun::Aiming()
{
	Accuracy /= 2;
}

void AGun::StopAiming()
{
	Accuracy *= 2;
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

// ------------------ Fire Animation -------------------------------------
void AGun::ServerMontageOnFire_Implementation() // �������� ������ �ִϸ��̼��� ó���ϴ� �Լ�
{
	if (Fire_Montage)
	{
		if (Mesh && Mesh->GetAnimInstance())
		{
			UE_LOG(LogTemp, Warning, TEXT("find AnimInstance"));
			Mesh->GetAnimInstance()->Montage_Play(Fire_Montage);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("find not AnimInstance"));
		}
		MultiCastMontage_Fire();
	}
}

bool AGun::ServerMontageOnFire_Validate()
{
	return true; 
}

void AGun::MultiCastMontage_Fire_Implementation() // ��� Ŭ���̾�Ʈ���� ������ ������ �ִϸ��̼��� ����ϵ��� �ϴ� ��Ƽĳ��Ʈ �Լ�. Ŭ���̾�Ʈ�� ���� ���� ����ȭ�� ���� ���
{
	if (Fire_Montage)
	{
		if (Mesh && Mesh->GetAnimInstance())
		{
			Mesh->GetAnimInstance()->Montage_Play(Fire_Montage);
		}
	}
}

// ------------------------------- Reload Animation -----------------------------
void AGun::ServerMontageOnReload_Implementation() // �������� ������ �ִϸ��̼��� ó���ϴ� �Լ�
{
	if (Reload_Montage)
	{
		if (Mesh && Mesh->GetAnimInstance())
		{
			UE_LOG(LogTemp, Warning, TEXT("find AnimInstance"));
			Mesh->GetAnimInstance()->Montage_Play(Reload_Montage);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("find not AnimInstance"));
		}
		MultiCastMontage_Reload();
	}
}

bool AGun::ServerMontageOnReload_Validate()
{
	return true;
}

void AGun::MultiCastMontage_Reload_Implementation() // ��� Ŭ���̾�Ʈ���� ������ ������ �ִϸ��̼��� ����ϵ��� �ϴ� ��Ƽĳ��Ʈ �Լ�. Ŭ���̾�Ʈ�� ���� ���� ����ȭ�� ���� ���
{
	if (Reload_Montage)
	{
		if (Mesh && Mesh->GetAnimInstance())
		{
			Mesh->GetAnimInstance()->Montage_Play(Reload_Montage);
		}
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

