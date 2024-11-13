// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Actor/GnuProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/GnuMyCharacter.h"
#include "Monster/GnuMonster.h"
#include "Kismet/GameplayStatics.h"

AGnuProjectileActor::AGnuProjectileActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->InitialSpeed = 500.f;  // 발사 속도
    ProjectileMovementComponent->MaxSpeed = 500.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;  // 발사체가 이동 방향으로 회전
    ProjectileMovementComponent->bShouldBounce = false;  // 바운스 설정 (필요시 true)
    ProjectileMovementComponent->ProjectileGravityScale = 0.f; // 중력 비활성화

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Block); // 모든 채널에 대해 충돌 허용
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ProjectileBoxError"));
    }


     NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
     NiagaraComponent->SetupAttachment(BoxComponent);


    Damage = -10.0f;
}

void AGnuProjectileActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (ProjectileMovementComponent)
    {
        BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true);

        // 입력된 방향을 기준으로 초기 속도 설정
        //ProjectileMovementComponent->Velocity = Direction * movementComp->InitialSpeed;

        FVector ForwardVector = ArrowComponent->GetForwardVector();
        ProjectileMovementComponent->Velocity = ForwardVector * ProjectileMovementComponent->InitialSpeed; // 각 방향, 속도로 발사
        FRotator Rotation = ForwardVector.Rotation();
        SetActorRotation(Rotation);
        ProjectileMovementComponent->Activate();  // 이동 시작

        //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("ForwardVector: %s"), *ForwardVector.ToString()));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Not ProjectileMoveComponent")));
    }
}

void AGnuProjectileActor::DestroyActor()
{
    Destroy();
}

void AGnuProjectileActor::BeginPlay()
{
    Super::BeginPlay();
    // 이전 위치를 초기 위치로 설정
    PreviousLocation = GetActorLocation();

    // 초기 속도 확인
    if (ProjectileMovementComponent)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("Projectile Velocity: %s"), *ProjectileMovementComponent->Velocity.ToString()));
    }

    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentHit.AddDynamic(this, &AGnuProjectileActor::OnHit);
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuProjectileActor::BeginOverlap); // BeginOverlap 이벤트 바인딩
    }

    if (NiagaraComponent)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Niagara Component Activated"));
        NiagaraComponent->Activate();
    }

    // 일정 시간 이후 firball actor 삭제 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuProjectileActor::DestroyActor, 10.0f, false);
}

void AGnuProjectileActor::Tick(float DeltaTime)
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

void AGnuProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때 엑터 삭제
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Overlap with: ") + OtherActor->GetName());
        Destroy();
    }
    if (OtherActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Arrow Hit!"));


            AController* OwnerController = GetInstigator()->GetController();
            if (OwnerController)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("OwnerController")));
                if (HasAuthority())
                {
                    UE_LOG(LogTemp, Warning, TEXT("HasAuthority!"));
                    UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("ApplyDamage")));
                    }

                }
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Not OwnerController")));
            }
        Destroy();
    }
}

void AGnuProjectileActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGnuProjectileActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
