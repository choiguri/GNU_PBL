// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Actor/GnuGrenadeActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/GnuMyCharacter.h"
#include "Monster/GnuMonster.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AGnuGrenadeActor::AGnuGrenadeActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널에 대해 충돌 허용
    }

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);

    //SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    //SphereComponent->SetupAttachment(NiagaraComponent);

    //// 물리 설정 (필요에 따라 물리 적용 여부 설정)
    //SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //SphereComponent->SetCollisionResponseToAllChannels(ECR_Block);

    Damage = -10.0f;
}

void AGnuGrenadeActor::BeginPlay()
{
    Super::BeginPlay();

    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentHit.AddDynamic(this, &AGnuGrenadeActor::OnHit);
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuGrenadeActor::BeginOverlap); // BeginOverlap 이벤트 바인딩
    }

    // 일정 시간 이후 firball actor 삭제 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &AGnuGrenadeActor::DestroyActor, 5.0f, false);
}

void AGnuGrenadeActor::Tick(float DeltaTime)
{

}

void AGnuGrenadeActor::LaunchProjectile(AActor* IgnoredActor, FVector SpawnLocation, FRotator SpawnRotation)
{
    AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(IgnoredActor);
    if (MyCharacter)
    {
        // 캐릭터의 SkeletalMeshComponent를 가져오기
        USkeletalMeshComponent* SkeletalMeshComponent = MyCharacter->GetMesh();

        if (MuzzleNiagaraSystem)
        {
            NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
                MuzzleNiagaraSystem,    // UNiagaraSystem* 타입의 Niagara 시스템 자산
                SkeletalMeshComponent,           // Niagara 시스템을 부착할 BoxComponent
                FName("WeaponSocket"),              // 소켓 이름 (None이면 기본 위치에 부착)
                FVector(0.0f, 80.0f, 10.0f),    // 위치 오프셋
                FRotator::ZeroRotator,  // 회전 오프셋
                EAttachLocation::SnapToTarget,  // 위치 기준 설정
                false,                   // 부모가 삭제되면 Niagara 시스템도 삭제
                true                    // 자동으로 활성화
            );
        }

        // 1초 후에 SpawnActor 호출하도록 타이머 설정
        FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
            this,
            &AGnuGrenadeActor::SpawnEMPVortex,
            IgnoredActor,
            SpawnLocation,
            SpawnRotation
        );
        GetWorld()->GetTimerManager().SetTimer(GrenadeTimer, TimerDelegate, 1.0f, false);
    }
}

void AGnuGrenadeActor::SpawnEMPVortex(AActor* IgnoredActor, FVector SpawnLocation, FRotator SpawnRotation)
{
     // 카메라 회전값을 가져오기
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

    if (CameraManager)
    {
        // 캐릭터가 바라보는 방향과 발사 위치를 설정
        AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(IgnoredActor); // 발사를 원하는 캐릭터
        if (MyCharacter)
        {
            // 캐릭터의 SkeletalMeshComponent를 가져오기
            USkeletalMeshComponent* SkeletalMeshComponent = MyCharacter->GetMesh();

            // 캐릭터의 현재 위치에서 발사
            FVector CharacterSpawnLocation = SkeletalMeshComponent->GetSocketLocation(FName("WeaponSocket")); // "WeaponSocket"을 원하는 소켓으로 변경

            FVector ProjectileOffset = FVector(170.0f, 0.0f, 0.0f);  // 발사 위치 오프셋

            // 카메라의 회전값을 얻어옴
            FRotator CameraRotation = CameraManager->GetCameraRotation();

            // 카메라의 회전값을 바탕으로 발사체 방향 계산
            FVector RotatedDirection = CameraRotation.RotateVector(ProjectileOffset);

            // 발사 위치 업데이트 (움직이는 캐릭터의 위치에 맞춰)
            FVector UpdatedSpawnLocation = CharacterSpawnLocation + RotatedDirection;

            // 카메라의 회전값을 SpawnRotation에 반영
            FRotator AdjustedRotation = CameraRotation;  // 카메라가 바라보는 방향으로 설정

            // 발사체를 해당 위치에서 발사
            FTransform Transform = UKismetMathLibrary::MakeTransform(UpdatedSpawnLocation, AdjustedRotation);

            // 액터 스폰 파라미터 설정
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this; // Owner 설정 (필요시 설정)

            // 발사체 액터 스폰
            AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
                BP_Projectile_EMP_Vortex,  // 스폰할 액터의 클래스
                Transform,
                SpawnParams
            );
        }
    }
}

void AGnuGrenadeActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AGnuGrenadeActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGnuGrenadeActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AGnuGrenadeActor::DestroyActor()
{
    // 타이머 중지
    GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
    GetWorld()->GetTimerManager().ClearTimer(GrenadeTimer);

    // 소멸 이펙트 재생
    NiagaraComponent->SetAutoDestroy(true); // 소멸 후 자동 삭제 설정
    NiagaraComponent->Deactivate();        // 이펙트 비활성화

    Destroy();
}

