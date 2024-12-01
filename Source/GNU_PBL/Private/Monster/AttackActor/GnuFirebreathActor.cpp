// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuFirebreathActor.h"
#include "Monster/GnuMonster.h"
#include "NiagaraComponent.h" // UNiagaraComponent 헤더 추가
#include "NiagaraFunctionLibrary.h" // Niagara 기능을 위한 헤더
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"


AGnuFirebreathActor::AGnuFirebreathActor()
{
	PrimaryActorTick.bCanEverTick = true;
    
    // ProjectileBoxComponent 생성 (기존 BoxComponent 아님)
    ProjectileBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileBoxComponent"));
    ProjectileBoxComponent->SetupAttachment(RootComponent);

    if (ProjectileBoxComponent)
    {
        // ProjectileBoxComponent 충돌 설정
        ProjectileBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 충돌과 물리 활성화
        ProjectileBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);          // 모든 채널 무시
        ProjectileBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);   // 캐릭터와 오버랩
        ProjectileBoxComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 정적 물체는 Block
    }

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = ProjectileBoxComponent; // ProjectileBoxComponent와 연결
    ProjectileMovement->InitialSpeed = 10000.f;                  // 발사 속도
    ProjectileMovement->MaxSpeed = 15000.f;                      // 발사 최대 속도
    ProjectileMovement->bRotationFollowsVelocity = true;        // 발사체가 이동 방향으로 회전
    ProjectileMovement->bShouldBounce = false;                  // 바운스 설정
    ProjectileMovement->ProjectileGravityScale = 0.f;           // 중력 비활성화

    // NiagaraComponent 초기화 및 설정
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);
    //NiagaraComponent->SetIsReplicated(true);  // 네트워크에서 복제 활성화

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널에 대해 충돌 허용
        //BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }

    // 상속 받아 데미지 정의
    DamageType = UDamageType::StaticClass();
    Damage = GetDamage();
}

void AGnuFirebreathActor::BeginPlay()
{
    Super::BeginPlay();

    // 초기화 시 파티클 활성화
    if (NiagaraComponent && HasAuthority())
    {
        NiagaraComponent->Activate();
    }
    else
    {
        NiagaraComponent->Deactivate();
        DestroyActor();
    }

    if (ProjectileBoxComponent)
    {
        // BeginOverlap 이벤트 바인딩
        ProjectileBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuFirebreathActor::BeginOverlap);
    }
}

void AGnuFirebreathActor::LaunchProjectile(AActor* IgnoredActor, FVector* Location, FRotator* Rotation)
{
    if (ProjectileBoxComponent && ProjectileMovement)
    {
        // 위치 설정: ProjectileBoxComponent의 위치를 Location 인자에 맞게 설정
        if (Location)
        {
            ProjectileBoxComponent->SetWorldLocation(*Location);
        }
        
        ProjectileBoxComponent->IgnoreActorWhenMoving(IgnoredActor, true); // 자신과 충돌 무시

        // 회전 설정: FRotator를 FVector로 변환하여 발사 방향 설정
        FVector LaunchDirection = Rotation->Vector();  // FRotator를 FVector로 변환

        // 발사체 속도 설정
        ProjectileMovement->Velocity = LaunchDirection * ProjectileMovement->InitialSpeed;
    }
}

void AGnuFirebreathActor::DestroyFirebreath()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate();  // 파티클 비활성화
        NiagaraComponent->SetAutoDestroy(true); // 파티클이 끝나면 삭제
    }

    // 서버에서만 타이머를 설정하여 액터 삭제
    GetWorld()->GetTimerManager().SetTimer(DestroyActorTimerHandle, this, &AGnuFirebreathActor::DestroyActor, 2.0f, false);
}

void AGnuFirebreathActor::DestroyActor()
{
    Destroy(); // 액터 삭제
}

void AGnuFirebreathActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            AController* OwnerController = OwnerCharacter->Controller;
            UGameplayStatics::ApplyDamage(OtherActor, GetDamage(), OwnerController, this, DamageType);
            GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, TEXT("Apply Damage!!"));
        }

        GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Magenta, TEXT("Begin Overlap with : ") + OtherActor->GetName());
    }
}