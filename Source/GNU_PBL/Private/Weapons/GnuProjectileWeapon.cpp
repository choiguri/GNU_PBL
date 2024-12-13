// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/GnuProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

// CombatComponent 에서 LineTrace한 HitTarget 받아서 실행
void AGnuProjectileWeapon::Fire(const FVector& HitTarget)
{
	if (ProjectileType == EProjectileType::EPT_Bullet)
	{
		Super::Fire(HitTarget);
	}

	if (!HasAuthority()) return;

	if (ProjectileType == EProjectileType::EPT_Grenade)
	{
		Boom();
		return;
	}

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

void AGnuProjectileWeapon::Boom()
{
	//if (GrenadeSkillClass->ChargeNiagaraComponent)
	//{
	//	ChargeNiagaraComponent->DestroyComponent();
	//}


	//ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	//if (OwnerCharacter)
	//{
	//	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	//	if (PlayerController)
	//	{
	//		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

	//		// 캐릭터의 SkeletalMeshComponent를 가져오기
	//		USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();

	//		// 캐릭터의 현재 위치에서 발사
	//		FVector CharacterSpawnLocation = SkeletalMeshComponent->GetSocketLocation(FName("WeaponSocket")); // "WeaponSocket"을 원하는 소켓으로 변경

	//		FVector ProjectileOffset = FVector(10.0f, 0.0f, 0.0f);  // 발사 위치 오프셋

	//		// 카메라의 회전값을 얻어옴
	//		FRotator CameraRotation = CameraManager->GetCameraRotation();

	//		// 카메라의 회전값을 바탕으로 발사체 방향 계산
	//		FVector RotatedDirection = CameraRotation.RotateVector(ProjectileOffset);

	//		// 발사 위치 업데이트 (움직이는 캐릭터의 위치에 맞춰)
	//		FVector UpdateSpawnLocation = CharacterSpawnLocation + RotatedDirection;

	//		// 카메라의 회전값을 SpawnRotation에 반영
	//		FRotator AdjustRotation = CameraRotation;  // 카메라가 바라보는 방향으로 설정

	//		// 발사체를 해당 위치에서 발사
	//		FTransform Transform = UKismetMathLibrary::MakeTransform(UpdateSpawnLocation, AdjustRotation);

	//		// 액터 스폰 파라미터 설정
	//		FActorSpawnParameters SpawnParams;
	//		SpawnParams.Owner = this; // Owner 설정 (필요시 설정)

	//		// 발사체 액터 스폰
	//		GetWorld()->SpawnActor<AGnuProjectile>(SeletedClass, Transform, SpawnParams);
	//		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, FString("Spawn"));
	//	}

	//}
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