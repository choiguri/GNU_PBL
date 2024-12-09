// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuFiretornadoActor.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AGnuFiretornadoActor::AGnuFiretornadoActor()
{
    // 와 미친 이거 하나 때문에 안날라가고 있었음;;
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true; // 복제 활성화

    // 이동 복제 활성화 (bReplicateMovement 대신)
    SetReplicateMovement(true);

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1000.f;  // 발사 속도
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;  // 발사체가 이동 방향으로 회전
    ProjectileMovement->bShouldBounce = false;  // 바운스 설정 (필요시 true)
    ProjectileMovement->ProjectileGravityScale = 0.f; // 중력 비활성화

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널에 대해 무시
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }
}

void AGnuFiretornadoActor::BeginPlay()
{
    Super::BeginPlay();

    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->IgnoreActorWhenMoving(this, true);    // 자신과의 충돌 무시
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuFiretornadoActor::BeginOverlap);    // 겹침 이벤트 바인딩
    }

    // 10초 이후 firball actor 삭제 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuFiretornadoActor::DestroyFiretornado, 10.0f, false);
}

void AGnuFiretornadoActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (ProjectileMovement && HasAuthority())
    {
        BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true); // 몬스터와 충돌 무시

        FVector ForwardVector = Arrow->GetForwardVector();
        ProjectileMovement->Velocity = ForwardVector * ProjectileMovement->InitialSpeed; // 각 방향, 속도로 발사
    }
}

void AGnuFiretornadoActor::DestroyFiretornado()
{
    Destroy();
}

void AGnuFiretornadoActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            AController* OwnerController = OwnerCharacter->Controller;
            UGameplayStatics::ApplyDamage(OtherActor, GetDamage(), OwnerController, this, DamageType);
        }
    }
}