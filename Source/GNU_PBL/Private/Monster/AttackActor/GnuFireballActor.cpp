// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuFireballActor.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AGnuFireballActor::AGnuFireballActor()
{
    // 와 미친 이거 하나 때문에 안날라가고 있었음;;
    PrimaryActorTick.bCanEverTick = true;

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 3000.f;                  // 발사 속도
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;        // 발사체가 이동 방향으로 회전
    ProjectileMovement->bShouldBounce = false;                  // 바운스 설정
    ProjectileMovement->ProjectileGravityScale = 0.f;           // 중력 비활성화

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널에 대해 충돌 허용
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }

    // 데미지 정의
    DamageType = UDamageType::StaticClass();
    Damage = 10.0f;
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
        /*BoxComponent->OnComponentHit.AddDynamic(this, &AGnuFireballActor::OnHit);*/
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuFireballActor::BeginOverlap); // BeginOverlap 이벤트 바인딩
    }

    //발사체 충돌 함수를 히트 이벤트에 등록
    /*if (GetLocalRole() == ROLE_Authority)
    {
        BoxComponent->OnComponentHit.AddDynamic(this, &AGnuFireballActor::OnProjectileImpact);
    }*/

    // 일정 시간 이후 firball actor 삭제 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuFireballActor::DestroyFireball, 5.0f, false);
}

void AGnuFireballActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (ProjectileMovement)
    {
        BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true); // 자신과 충돌 무시

        FVector ForwardVector = Arrow->GetForwardVector();
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("ForwardVector: %s"), *ForwardVector.ToString()));
        ProjectileMovement->Velocity = ForwardVector * ProjectileMovement->InitialSpeed; // 각 방향, 속도로 발사
    }
}

void AGnuFireballActor::DestroyFireball()
{
    Destroy();
}

// Damage 구현부
void AGnuFireballActor::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor)
    {
        /*UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);*/
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Impact with: ") + OtherActor->GetName());
    }
}

void AGnuFireballActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때 파이어볼 삭제
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Hit with: ") + OtherActor->GetName());
        /*Destroy();*/
    }
}

void AGnuFireballActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {

        // 충돌한 액터가 벽이나 캐릭터일 때 파이어볼 삭제
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            AController* OwnerController = OwnerCharacter->Controller;
            UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, DamageType);
            GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, TEXT("Apply Damage!!"));
        }

        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Overlap with: ") + OtherActor->GetName());
    }
}