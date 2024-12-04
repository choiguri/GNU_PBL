// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuGroundActor.h"
#include "NiagaraComponent.h" // UNiagaraComponent 헤더 추가
#include "NiagaraFunctionLibrary.h" // Niagara 기능을 위한 헤더
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Monster/GnuMonster.h"
#include "Characters/GnuMyCharacter.h"
#include "Stats/Stats.h"


AGnuGroundActor::AGnuGroundActor()
{
	PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(true);

    RootComponent = BoxComponent;

    Monster = nullptr;


    // ProjectileBoxComponent 생성 (기존 BoxComponent 아님)
    ProjectileBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileBoxComponent"));
    ProjectileBoxComponent->SetupAttachment(RootComponent);

    if (ProjectileBoxComponent)
    {
        // ProjectileBoxComponent 충돌 설정
        ProjectileBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 충돌과 물리 활성화
        ProjectileBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);          // 모든 채널 무시
        ProjectileBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);   // 캐릭터와 오버랩
    }

    // ProjectileMovementComponent 생성 및 기본값 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = ProjectileBoxComponent; // ProjectileBoxComponent와 연결
    ProjectileMovement->InitialSpeed = 5000.f;                  // 발사 속도
    ProjectileMovement->MaxSpeed = 7000.f;                      // 발사 최대 속도
    ProjectileMovement->bRotationFollowsVelocity = true;        // 발사체가 이동 방향으로 회전
    ProjectileMovement->bShouldBounce = false;                  // 바운스 설정
    ProjectileMovement->ProjectileGravityScale = 0.f;           // 중력 비활성화


	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(BoxComponent);

    // 상속 받아 데미지 정의
    DamageType = UDamageType::StaticClass();
    Damage = GetDamage();
}


void AGnuGroundActor::BeginPlay()
{
    Super::BeginPlay();

    // 초기 위치 저장
    InitialLocation = GetActorLocation();

    // GetOwner()로 소유자에서 AGnuMonster를 찾을 수 있는지 확인
    AActor* OwnerActor = GetOwner();
    if (OwnerActor)
    {
        Monster = Cast<AGnuMonster>(OwnerActor);
        if (Monster)
        {
            UE_LOG(LogTemp, Warning, TEXT("Monster assigned successfully!"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to assign Monster"));
        }
    }


    // 초기화 시 파티클 활성화
    if (NiagaraComponent && HasAuthority())
    {
        NiagaraComponent->Activate();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DestroyGround called prematurely"));
        NiagaraComponent->Deactivate();
        Destroy();
    }

    if (ProjectileBoxComponent)
    {
        ProjectileBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGnuGroundActor::BeginOverlap);
    }

    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuGroundActor::DestroyGround, 3.0f, false);
}

void AGnuGroundActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ProjectileBoxComponent && ProjectileMovement)
    {
        float TravelDistance = FVector::Dist(InitialLocation, ProjectileBoxComponent->GetComponentLocation());
        /*GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Travel Distance: %f"), TravelDistance));*/
        if (TravelDistance >= MaxDistance)
        {
            DestroyBox();
        }
    }
}

void AGnuGroundActor::DestroyGround()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate(); // 파티클 시스템 비활성화
        NiagaraComponent->SetAutoDestroy(true); // 파티클이 끝나면 삭제
    }

    Destroy(); // 액터 삭제
}

void AGnuGroundActor::DestroyBox()
{
    if (ProjectileBoxComponent)
    {
        ProjectileBoxComponent->DestroyComponent();
    }
}

void AGnuGroundActor::LaunchProjectile(AActor* IgnoredActor, FVector* Location, FRotator* Rotation)
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

void AGnuGroundActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && (OtherActor != this))
    {
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            AController* OwnerController = OwnerCharacter->Controller;
            UGameplayStatics::ApplyDamage(OtherActor, GetDamage(), OwnerController, this, DamageType);

            AGnuMyCharacter* TargetCharacter = Cast<AGnuMyCharacter>(OtherActor);
            if (TargetCharacter)
            {
                if (Monster)
                {
                    Monster->KnockbackStrength = 10000.f;
                    Monster->KnockbackPlayer(TargetCharacter);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Monster is nullptr"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("TargetCharacter cast failed"));
            }



            GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Black, TEXT("Apply Damage!!"));
        }

        GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Begin Overlap with : ") + OtherActor->GetName());
    }
}