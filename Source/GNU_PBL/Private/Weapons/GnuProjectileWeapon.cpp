// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/GnuProjectile.h"
#include "Net/UnrealNetwork.h"

// CombatComponent 에서 LineTrace한 HitTarget 받아서 실행
void AGnuProjectileWeapon::Fire(const FVector& HitTarget)
{
	if (ProjectileType == EProjectileType::EPT_Bullet)
	{
		Super::Fire(HitTarget);
	}

	/*if (!HasAuthority()) return;*/

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlashSocket"));

	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

		// Muzzle Flash Socket to hit Location from TraceUnderCrosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation(); 
		FRotator TargetRotation = ToTarget.Rotation();


		if (SeletedClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AGnuProjectile>(
					SeletedClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
	}

}

void AGnuProjectileWeapon::SetClass()
{
	if (ProjectileType == EProjectileType::EPT_Bullet)
	{
		SeletedClass = ProjectileClass;
	}
	else if (ProjectileType == EProjectileType::EPT_Razer)
	{
		SeletedClass = RazerSkillClass;
	}
	else if (ProjectileType == EProjectileType::EPT_Grenade)
	{
		SeletedClass = GrenadeSkillClass;
	}
}

void AGnuProjectileWeapon::SetProjectileType(EProjectileType SetType)
{
	if (HasAuthority())
	{
		ProjectileType = SetType;
		OnRep_ProjectileType(); // 서버에서도 직접 반영
	}
	else
	{
		// 클라이언트에서 서버에 요청
		ServerSetProjectileType(SetType);
	}
}

void AGnuProjectileWeapon::ServerSetProjectileType_Implementation(EProjectileType SetType)
{
	ProjectileType = SetType;
	OnRep_ProjectileType(); // Replication된 값 반영
}

bool AGnuProjectileWeapon::ServerSetProjectileType_Validate(EProjectileType SetType)
{
	return true; // 기본적으로 항상 유효
}

void AGnuProjectileWeapon::OnRep_ProjectileType()
{
	SetClass(); // ProjectileType 변경에 따라 SeletedClass 업데이트
}

void AGnuProjectileWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGnuProjectileWeapon, ProjectileType);
}