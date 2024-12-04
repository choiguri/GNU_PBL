// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/GnuProjectile.h"


// CombatComponent 에서 LineTrace한 HitTarget 받아서 실행
void AGnuProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlashSocket"));

	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

		// Muzzle Flash Socket to hit Location from TraceUnderCrosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation(); 
		FRotator TargetRotation = ToTarget.Rotation();


		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AGnuProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
	}

}
