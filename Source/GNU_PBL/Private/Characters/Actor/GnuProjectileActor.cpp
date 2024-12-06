#include "Characters/Actor/GnuProjectileActor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"
#include "GNU_PBL/GNU_PBL.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AGnuProjectileActor::AGnuProjectileActor()
{
    PrimaryActorTick.bCanEverTick = true;

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);
    NiagaraComponent->SetIsReplicated(true);
    NiagaraComponent->SetAutoDestroy(true);

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComp->InitialSpeed = 2000.f;                  // 발사 속도
    ProjectileMovementComp->MaxSpeed = 2000.f;                      // 발사 최대 속도
    ProjectileMovementComp->bRotationFollowsVelocity = true;        // 발사체가 이동 방향으로 회전
    ProjectileMovementComp->bShouldBounce = false;                  // 바운스 설정
    ProjectileMovementComp->ProjectileGravityScale = 0.f;           // 중력 비활성화

    BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    BoxComponent->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

    Damage = 10.0f;

    bReplicates = true;  // 네트워크에서 복제할 수 있도록 설정
}

void AGnuProjectileActor::BeginPlay()
{
    Super::BeginPlay();

    if(NiagaraComponent)
    { 
        NiagaraComponent->Activate();
    }

    BoxComponent->OnComponentHit.AddDynamic(this, &AGnuProjectileActor::OnHit);
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuProjectileActor::BeginOverlap);

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGnuProjectileActor::DestroyActor, 1.5f, false);

    if (FlyComponent) // 날아가나는 이펙트만 박스에 붙혀서 날림
    {
        NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            FlyComponent,
            BoxComponent,
            FName(),
            GetActorLocation(),
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition,
            true
        );
    }
    
    if (MuzzleComponent) // 머즐 이펙트는 시작하는 위치에 바로 생성시킴
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            MuzzleComponent,
            GetActorLocation(),
            GetActorRotation()
        );
    }

    if (NiagaraComponent)
    {
        // 크기 조정
        NiagaraComponent->SetWorldScale3D(FVector(1.3f, 1.3f, 1.3f)); // 2배 크기
    }
}

void AGnuProjectileActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGnuProjectileActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (HasAuthority())
    {
        if (BoxComponent)
        {
            BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true);
            FVector LaunchDirection = GetActorForwardVector();
            ProjectileMovementComp->Velocity = LaunchDirection * ProjectileMovementComp->InitialSpeed;
        }
    }
    else
    {
        ServerLaunchProjectile(IgnoredActor); // Call server function if client
    }
}

void AGnuProjectileActor::ServerLaunchProjectile_Implementation(AActor* IgnoredActor)
{
    LaunchProjectile(IgnoredActor);
}

bool AGnuProjectileActor::ServerLaunchProjectile_Validate(AActor* IgnoredActor)
{
    return true;  // Validate the request if necessary
}

void AGnuProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (HasAuthority())
    {
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            AController* OwnerController = OwnerCharacter->Controller;
            if (OwnerController)
            {
                UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());

                NiagaraComponent->DestroyComponent(); // Fly effect destroy
                MulticastDestroyFlyEffect();

                if (TargetComponent)
                {
                    if (HasAuthority())
                    {
                        MulticastSpawnEffect(TargetComponent, GetActorLocation(), GetActorRotation());
                    }
                    else
                    {
                        ServerOnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
                    }
                }
            }
        }
    }
    else
    {
        ServerOnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit); // Call server function if client
    }
}

// Implement the server function for OnHit
void AGnuProjectileActor::ServerOnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}

bool AGnuProjectileActor::ServerOnHit_Validate(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    return true;  // Validate if necessary
}

void AGnuProjectileActor::MulticastSpawnEffect_Implementation(UNiagaraSystem* NiagaraSystem, FVector Location, FRotator Rotation)
{
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, Location, Rotation);
}

void AGnuProjectileActor::MulticastDestroyFlyEffect_Implementation()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate();
        NiagaraComponent->DestroyComponent();
    }
}

void AGnuProjectileActor::DestroyActor()
{
    if (HasAuthority())
    {
        MulticastDestroyFlyEffect();
    }
    Destroy(); // Destroy actor
}

void AGnuProjectileActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGnuProjectileActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}