// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuFireballActor.h"
#include "Monster/GnuMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h" // UNiagaraComponent 헤더 추가
#include "NiagaraFunctionLibrary.h" // Niagara 기능을 위한 헤더
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AGnuFireballActor::AGnuFireballActor()
{
    // 와 미친 이거 하나 때문에 안날라가고 있었음;;
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true; // 복제 활성화

    // 이동 복제 활성화 (bReplicateMovement 대신)
    SetReplicateMovement(true);

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 3000.f;                  // 발사 속도
    ProjectileMovement->MaxSpeed = 4000.f;                      // 최고 속도
    ProjectileMovement->bRotationFollowsVelocity = true;        // 발사체가 이동 방향으로 회전
    ProjectileMovement->bShouldBounce = false;                  // 바운스 설정
    ProjectileMovement->ProjectileGravityScale = 0.f;           // 중력 비활성화

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널에 대해 무시
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }
}

void AGnuFireballActor::BeginPlay()
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
        Destroy();
    }

    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuFireballActor::BeginOverlap); // BeginOverlap 이벤트 바인딩
    }

    // 일정 시간 이후 firball actor 삭제 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuFireballActor::DestroyFireball, 3.0f, false);
}

void AGnuFireballActor::LaunchProjectile(AActor* IgnoredActor)
{
    if (ProjectileMovement && HasAuthority())  // 서버에서만 실행되도록
    {
        BoxComponent->IgnoreActorWhenMoving(IgnoredActor, true); // 자신과 충돌 무시

        FVector ForwardVector = Arrow->GetForwardVector();
        ProjectileMovement->Velocity = ForwardVector * ProjectileMovement->InitialSpeed; // 서버에서 발사체의 속도 설정
    }
}

void AGnuFireballActor::DestroyFireball()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate(); // 파티클 시스템 비활성화
        NiagaraComponent->SetAutoDestroy(true); // 파티클이 끝나면 삭제
    }

    Destroy(); // 액터 삭제
}


// 데미지 구현부
void AGnuFireballActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            AController* OwnerController = OwnerCharacter->Controller;
            UGameplayStatics::ApplyDamage(OtherActor, GetDamage(), OwnerController, this, DamageType);
            /*GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Black, TEXT("Apply Damage!!"));*/
        }
        /*GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Begin Overlap with : ") + OtherActor->GetName());*/
    }
}