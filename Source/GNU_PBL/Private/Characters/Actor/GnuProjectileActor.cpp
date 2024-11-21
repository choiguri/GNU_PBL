// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Actor/GnuProjectileActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


AGnuProjectileActor::AGnuProjectileActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌을 비활성화
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);  // 모든 채널에 대해 충돌 비허용
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);  // 캐릭터에 대해 충돌 비허용
    }

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);

    ProjectileBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileBoxComponent"));
    ProjectileBoxComponent->SetupAttachment(BoxComponent);

    if (ProjectileBoxComponent)
    {
        ProjectileBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        ProjectileBoxComponent->SetCollisionResponseToAllChannels(ECR_Block); // 모든 채널에 대해 충돌 허용
        ProjectileBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }

    Damage = -10.0f;
}

void AGnuProjectileActor::BeginPlay()
{
    Super::BeginPlay();
    // 이전 위치를 초기 위치로 설정
    PreviousLocation = GetActorLocation();

    // 충돌을 위해 이벤트 바인딩
    if (ProjectileBoxComponent)
    {
        ProjectileBoxComponent->OnComponentHit.AddDynamic(this, &AGnuProjectileActor::OnHit);
        ProjectileBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuProjectileActor::BeginOverlap); // BeginOverlap 이벤트 바인딩
    }
    // 일정 시간 이후 firball actor 삭제 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGnuProjectileActor::DestroyActor, 5.0f, false);
}

void AGnuProjectileActor::Tick(float DeltaTime)
{
    float Speed = 500.0f;

    // BoxComponent의 현재 위치 가져오기
    FVector CurrentLocation = BoxComponent->GetComponentLocation();

    // 발사체의 전방 방향 (기본적으로 Forward 벡터를 사용)
    FVector ForwardDirection = GetActorForwardVector();

    // 새로운 위치 계산 (속도 * DeltaTime)
    FVector NewLocation = CurrentLocation + (ForwardDirection * Speed * DeltaTime);

    // 위치 갱신
    ProjectileBoxComponent->SetWorldLocation(NewLocation);
    // 이전 위치를 현재 위치로 업데이트
    PreviousLocation = CurrentLocation;
}

void AGnuProjectileActor::LaunchProjectile(AActor* IgnoredActor)
{
    ProjectileBoxComponent->IgnoreActorWhenMoving(IgnoredActor, true);
}

void AGnuProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때 엑터 삭제
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Overlap with: ") + OtherActor->GetName());

        Destroy();
    }
}

void AGnuProjectileActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGnuProjectileActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AGnuProjectileActor::DestroyActor()
{
    Destroy();
}