// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuFireballActor.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

AGnuFireballActor::AGnuFireballActor()
{
    // 와 미친 이거 하나 때문에 안날라가고 있었음;;
    PrimaryActorTick.bCanEverTick = true;

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1500.f;  // 발사 속도
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;  // 발사체가 이동 방향으로 회전
    ProjectileMovement->bShouldBounce = false;  // 바운스 설정 (필요시 true)
    ProjectileMovement->ProjectileGravityScale = 0.f; // 중력 비활성화

    //BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //BoxComponent->SetCollisionResponseToAllChannels(ECR_Block); // 모든 채널에 대해 충돌 허용
    //BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // 캐릭터에 대해 충돌 허용

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Block);
    }
}

void AGnuFireballActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (ProjectileMovement)
    {
        BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true); // 몬스터와 충돌 무시

        FVector ForwardVector = Arrow->GetForwardVector();
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("ForwardVector: %s"), *ForwardVector.ToString()));
        ProjectileMovement->Velocity = ForwardVector * ProjectileMovement->InitialSpeed; // 각 방향, 속도로 발사
    }
}



void AGnuFireballActor::BeginPlay()
{

    Super::BeginPlay();

    // 초기 속도 확인
    if (ProjectileMovement)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("Projectile Velocity: %s"), *ProjectileMovement->Velocity.ToString()));
    }


    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentHit.AddDynamic(this, &AGnuFireballActor::OnHit);
    }
}

void AGnuFireballActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때 파이어볼 삭제
        Destroy();
    }
}

void AGnuFireballActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}



void AGnuFireballActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
