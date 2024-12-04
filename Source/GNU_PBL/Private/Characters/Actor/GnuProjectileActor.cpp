#include "Characters/Actor/GnuProjectileActor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"
#include "GNU_PBL/GNU_PBL.h"
#include "Kismet/GameplayStatics.h"

AGnuProjectileActor::AGnuProjectileActor()
{
    PrimaryActorTick.bCanEverTick = true;
    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    //if (BoxComponent)
    //{
    //    BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //    BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널에 대해 충돌 허용
    //}

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);

    /*ProjectileBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileBoxComponent"));
    ProjectileBoxComponent->SetupAttachment(BoxComponent);*/

    //ProjectileBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //ProjectileBoxComponent->SetCollisionResponseToAllChannels(ECR_Block); // 모든 채널에 대해 충돌 허용
    //ProjectileBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComp->UpdatedComponent = ProjectileBoxComponent; // ProjectileBoxComponent와 연결
    ProjectileMovementComp->InitialSpeed = 2000.f;                  // 발사 속도
    ProjectileMovementComp->MaxSpeed = 2000.f;                      // 발사 최대 속도
    ProjectileMovementComp->bRotationFollowsVelocity = true;        // 발사체가 이동 방향으로 회전
    ProjectileMovementComp->bShouldBounce = false;                  // 바운스 설정
    ProjectileMovementComp->ProjectileGravityScale = 0.f;           // 중력 비활성화

   /* ProjectileBoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    ProjectileBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ProjectileBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    ProjectileBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    ProjectileBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    ProjectileBoxComponent->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);*/

    BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    BoxComponent->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

    Damage = 10.0f;
}

void AGnuProjectileActor::BeginPlay()
{
    Super::BeginPlay();
    PreviousLocation = GetActorLocation();

    if(NiagaraComponent)
    { 
        NiagaraComponent->Activate();
    }

    BoxComponent->OnComponentHit.AddDynamic(this, &AGnuProjectileActor::OnHit);
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuProjectileActor::BeginOverlap);

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGnuProjectileActor::DestroyActor, 2.5f, false);

    if (NiagaraSystem)
    {
        NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            NiagaraSystem,
            BoxComponent,
            FName(),
            GetActorLocation(), //getactor locatoin
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition,
            true
        );
        /*NiagaraComponent->SetVectorParameter(TEXT("Scale Added Velocity"), FVector(1.f, 1.f, 1.f));
        NiagaraComponent->SetVectorParameter(TEXT("Velocity"), FVector(0.f, 0.f, 0.f));*/

       /* NiagaraComponent->SetVectorParameter(TEXT("Scale Added Velocity"), FVector(1.f, 1.f, 1.f));
        NiagaraComponent->SetVectorParameter(TEXT("Velocity"), FVector(1000.f, 0.f, 0.f));*/
    }
}

void AGnuProjectileActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGnuProjectileActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (BoxComponent)
    {
        BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true); // 몬스터와 충돌 무시
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString("LaunchProjectile!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
        FVector LaunchDirection = GetActorForwardVector();
        ProjectileMovementComp->Velocity = LaunchDirection * ProjectileMovementComp->InitialSpeed;
    }
}

void AGnuProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        AController* OwnerController = OwnerCharacter->Controller;
        if (OwnerController)
        {
            UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
        }
    }
}

void AGnuProjectileActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Overlap with: ") + OtherActor->GetName());
}

void AGnuProjectileActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AGnuProjectileActor::DestroyActor()
{
    GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString("DestroyActor!!!!!!!!!!!!!!!!!!!!!!!!!!!"));

    // 소멸 이펙트 재생
    NiagaraComponent->SetAutoDestroy(true); // 소멸 후 자동 삭제 설정
    NiagaraComponent->Deactivate();        // 이펙트 비활성화
    Destroy();
}