// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuGrenadeProjectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

void AGnuGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();

    if (ChargeNiagara)
    {
        ChargeNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            ChargeNiagara,
            SkeletalMeshComponent,
            FName("WeaponSocket"),
            FVector(0.0f, 80.0f, 10.0f),
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            false,
            true
        );
    }

    /*GetWorld()->SpawnActor(BP_Grenade);*/
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this,&AGnuGrenadeProjectile::Boom);

	GetWorld()->GetTimerManager().SetTimer(GrenadeTimer, TimerDelegate, 1.0f, false);
}
//
//void AGnuGrenadeProjectile::Boom()
//{
//    GetWorld()->GetTimerManager().ClearTimer(GrenadeTimer);
//    if (ChargeNiagaraComponent)
//    {
//        ChargeNiagaraComponent->DestroyComponent();
//    }
//    
//   /* APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);*/
//    /*APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;*/
//
//    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
//    if (OwnerCharacter)
//    {
//        APlayerController* OwnerController = Cast<APlayerController>(OwnerCharacter->GetController());
//        if (OwnerController)
//        {
//             APlayerCameraManager* CameraManager = OwnerController->PlayerCameraManager;
//             // 발사 위치 및 회전값 계산
//             USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
//             FVector CharacterSpawnLocation = SkeletalMeshComponent->GetSocketLocation(FName("WeaponSocket"));
//             FVector ProjectileOffset = FVector(170.0f, 0.0f, 0.0f); // 발사 오프셋
//
//             if (CameraManager)
//             {
//                FRotator CameraRotation = CameraManager->GetCameraRotation();
//                FVector RotatedDirection = CameraRotation.RotateVector(ProjectileOffset);
//                FVector UpdateSpawnLocation = CharacterSpawnLocation + RotatedDirection;
//                FRotator AdjustRotation = CameraRotation;
//
//               SpawnGrenade(UpdateSpawnLocation, AdjustRotation, OwnerCharacter);
//
//             }
//
//        }
//    }
//}
//
//void AGnuGrenadeProjectile::SpawnGrenade(FVector Location, FRotator Rotation, ACharacter* OwnerCharacter)
//{
//    FTransform Transform = UKismetMathLibrary::MakeTransform(Location, Rotation);
//    FActorSpawnParameters SpawnParams;
//    SpawnParams.Owner = OwnerCharacter;
//    SpawnParams.Instigator = OwnerCharacter;
//    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BP_Grenade, Transform, SpawnParams);
//}
//
//
//void AGnuGrenadeProjectile::Server_SpawnGrenade_Implementation(FVector Location, FRotator Rotation, ACharacter* OwnerCharacter)
//{
//    /*Multicast_SpawnGrenade(Location, Rotation);*/
//    if (HasAuthority())
//    {
//        FTransform Transform = UKismetMathLibrary::MakeTransform(Location, Rotation);
//        FActorSpawnParameters SpawnParams;
//        SpawnParams.Owner = OwnerCharacter;
//        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BP_Grenade, Transform, SpawnParams);
//    }
//    else
//    {
//        /*Client_SpawnGrenade(Location, Rotation);*/
//        FTransform Transform = UKismetMathLibrary::MakeTransform(Location, Rotation);
//        FActorSpawnParameters SpawnParams;
//        SpawnParams.Owner = OwnerCharacter;
//        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BP_Grenade, Transform, SpawnParams);
//    }
//}
//
//void AGnuGrenadeProjectile::Multicast_SpawnGrenade_Implementation(FVector Location, FRotator Rotation)
//{
//    if (HasAuthority() || !IsNetMode(NM_DedicatedServer))
//    {
//        FTransform Transform = UKismetMathLibrary::MakeTransform(Location, Rotation);
//        FActorSpawnParameters SpawnParams;
//        SpawnParams.Owner = this;
//        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BP_Grenade, Transform, SpawnParams);
//    }
//}
//
//void AGnuGrenadeProjectile::Client_SpawnGrenade_Implementation(FVector Location, FRotator Rotation, ACharacter* OwnerCharacter)
//{
//    /*if (HasAuthority()) 
//    {
//        Server_SpawnGrenade(Location, Rotation);
//    }*/
//    FTransform Transform = UKismetMathLibrary::MakeTransform(Location, Rotation);
//    FActorSpawnParameters SpawnParams;
//    SpawnParams.Owner = OwnerCharacter;
//    SpawnParams.Instigator = OwnerCharacter;
//    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BP_Grenade, Transform, SpawnParams);
//}

void AGnuGrenadeProjectile::Boom()
{
    GetWorld()->GetTimerManager().ClearTimer(GrenadeTimer);
    if (ChargeNiagaraComponent)
    {
        ChargeNiagaraComponent->DestroyComponent();
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
        // 캐릭터의 SkeletalMeshComponent를 가져오기
        USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();

        // 캐릭터의 현재 위치에서 발사
        FVector CharacterSpawnLocation = SkeletalMeshComponent->GetSocketLocation(FName("WeaponSocket")); // "WeaponSocket"을 원하는 소켓으로 변경

        FVector ProjectileOffset = FVector(170.0f, 0.0f, 0.0f);  // 발사 위치 오프셋

        // 카메라의 회전값을 얻어옴
        FRotator CameraRotation = CameraManager->GetCameraRotation();

        // 카메라의 회전값을 바탕으로 발사체 방향 계산
        FVector RotatedDirection = CameraRotation.RotateVector(ProjectileOffset);

        // 발사 위치 업데이트 (움직이는 캐릭터의 위치에 맞춰)
        FVector UpdateSpawnLocation = CharacterSpawnLocation + RotatedDirection;

        // 카메라의 회전값을 SpawnRotation에 반영
        FRotator AdjustRotation = CameraRotation;  // 카메라가 바라보는 방향으로 설정

        // 발사체를 해당 위치에서 발사
        FTransform Transform = UKismetMathLibrary::MakeTransform(UpdateSpawnLocation, AdjustRotation);

        // 액터 스폰 파라미터 설정
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this; // Owner 설정 (필요시 설정)

        // 발사체 액터 스폰
        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(BP_Grenade, Transform, SpawnParams);

	}
}