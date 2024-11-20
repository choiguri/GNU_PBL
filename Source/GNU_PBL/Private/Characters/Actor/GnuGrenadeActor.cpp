// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Actor/GnuGrenadeActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/GnuMyCharacter.h"
#include "Monster/GnuMonster.h"
#include "Weapons/Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

AGnuGrenadeActor::AGnuGrenadeActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // RootSceneComponent 생성 및 설정
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComponent;

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Block); // 모든 채널에 대해 충돌 허용
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);

    ProjectileOffset = FVector(15.0f, 35.0f, -10.0f);
    Damage = -10.0f;
}

void AGnuGrenadeActor::BeginPlay()
{
    Super::BeginPlay();
    // 이전 위치를 초기 위치로 설정
    PreviousLocation = GetActorLocation();

    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentHit.AddDynamic(this, &AGnuGrenadeActor::OnHit);
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuGrenadeActor::BeginOverlap); // BeginOverlap 이벤트 바인딩
    }

    // 일정 시간 이후 firball actor 삭제 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuGrenadeActor::DestroyActor, 10.0f, false);


}

void AGnuGrenadeActor::Tick(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();


    // 이전 위치와 현재 위치 사이에 디버그 라인 그리기
    DrawDebugLine(
        GetWorld(),
        PreviousLocation,
        CurrentLocation,
        FColor::Red,          // 경로 색상
        false,                 // 영구 표시 여부
        -1.0f,                 // 지속 시간 (Tick마다 갱신이므로 -1.0f 사용)
        0,
        2.0f                   // 선 두께
    );

    // 이전 위치를 현재 위치로 업데이트
    PreviousLocation = CurrentLocation;
}

void AGnuGrenadeActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (MuzzleNiagaraSystem)
    {
        NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleNiagaraSystem,    // UNiagaraSystem* 타입의 Niagara 시스템 자산
            BoxComponent,           // Niagara 시스템을 부착할 BoxComponent
            NAME_None,              // 소켓 이름 (None이면 기본 위치에 부착)
            FVector(0.0f, 80.0f, 10.0f),    // 위치 오프셋
            FRotator::ZeroRotator,  // 회전 오프셋
            EAttachLocation::KeepRelativeOffset,  // 위치 기준 설정
            false,                   // 부모가 삭제되면 Niagara 시스템도 삭제
            true                    // 자동으로 활성화
        );
    }

    // 1초 후에 SpawnEMPVortex 메서드를 호출하도록 타이머 설정
    FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
        this,
        &AGnuGrenadeActor::SpawnEMPVortex,
        IgnoredActor
    );

    // TimerHandle을 멤버 변수로 사용
    GetWorld()->GetTimerManager().SetTimer(
        DestructionTimerHandle,  // 기존 클래스 멤버 변수 사용
        TimerDelegate,
        1.0f, // 1초 뒤에 실행
        false // 반복하지 않음
    );

    BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true);
}
void AGnuGrenadeActor::SpawnEMPVortex(AActor* IgnoredActor)
{
    if (BP_Projectile_EMP_Vortex) // 스폰할 액터의 클래스가 유효한지 확인
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        if (!PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is not available!"));
            return;
        }

        // 2. Get Player Camera Manager
        APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
        if (!CameraManager)
        {
            UE_LOG(LogTemp, Warning, TEXT("CameraManager is not available!"));
            return;
        }

        // MuzzleSocket 위치를 얻고, ProjectileOffset을 더해서 결과를 계산
        AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(IgnoredActor);
        USkeletalMeshComponent* SkeletalMeshComponent = MyCharacter ? MyCharacter->GetMesh() : nullptr;
        if (SkeletalMeshComponent)
        {
            FRotator CameraRotation = CameraManager->GetCameraRotation();
            FVector Temp = CameraRotation.RotateVector(ProjectileOffset);
            // 소켓 위치를 가져온 후 오프셋 추가
            FVector MoveSpawnLocation = SkeletalMeshComponent->GetSocketLocation(FName("MuzzleFlashSocket")) + ProjectileOffset;

            FTransform SpawnTransfrom = UKismetMathLibrary::MakeTransform(MoveSpawnLocation, CameraRotation);

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 충돌을 무시하고 액터 스폰

            // 액터 스폰
            GetWorld()->SpawnActor<AActor>(BP_Projectile_EMP_Vortex, SpawnTransfrom, SpawnParams);
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
}
