// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuGroundSpikeCollisionActor.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AGnuGroundSpikeCollisionActor::AGnuGroundSpikeCollisionActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1000.f;                  // 초기 발사 속도
    ProjectileMovement->MaxSpeed = 1000.f;                      // 최대 발사 속도
    ProjectileMovement->bRotationFollowsVelocity = true;        // 발사체가 이동 방향으로 회전
    ProjectileMovement->bShouldBounce = false;                  // 바운스 설정
    ProjectileMovement->ProjectileGravityScale = 0.f;           // 중력 비활성화

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널에 대해 무시
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }

    DamageType = UDamageType::StaticClass();
    Damage = 25.0f;
}

void AGnuGroundSpikeCollisionActor::BeginPlay()
{
    Super::BeginPlay();

    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuGroundSpikeCollisionActor::BeginOverlap); // BeginOverlap 이벤트 바인딩
    }

    // 일정 시간 이후 Actor 삭제
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuGroundSpikeCollisionActor::DestroyGroundSpikeCollision, 2.5f, false);
}

void AGnuGroundSpikeCollisionActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (ProjectileMovement)
    {
        BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true); // 자신과 충돌 무시

        FVector ForwardVector = Arrow->GetForwardVector();
        ProjectileMovement->Velocity = ForwardVector * ProjectileMovement->InitialSpeed; // 각 방향, 속도로 발사
    }
}

void AGnuGroundSpikeCollisionActor::DestroyGroundSpikeCollision()
{
    Destroy();
}

void AGnuGroundSpikeCollisionActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            AController* OwnerController = OwnerCharacter->Controller;
            UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, DamageType);
        }
    }
}
