// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Actor/GnuHealActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/GnuMyCharacter.h"
#include "TimerManager.h"  // 타이머 관련 클래스 포함
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AGnuHealActor::AGnuHealActor()
{
    PrimaryActorTick.bCanEverTick = true;

    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Block); // 모든 채널에 대해 충돌 허용
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
    NiagaraComponent->SetupAttachment(BoxComponent);
    Damage = -2.0f;
    bReplicates = true; // 액터 복제 가능 설정
}


void AGnuHealActor::BeginPlay()
{
    Super::BeginPlay();

    if (waveHealSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, waveHealSound, GetActorLocation(), 1.5f);
    }

    // 힐 시작
    GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &AGnuHealActor::DestroyActor, 5.0f, false);
    GetWorld()->GetTimerManager().SetTimer(HealTimer, this, &AGnuHealActor::Heal, 0.5f, true);
}

void AGnuHealActor::Tick(float DeltaTime)
{
    // Niagara 이펙트를 계속해서 캐릭터 위치에 맞춰 업데이트 (캐릭터와 함께 움직이도록)
    // Overlap 된 캐릭터를 계속 추적하여 이펙트를 캐릭터 위치로 이동
    if (AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(GetOwner()))
    {
        FVector CharacterLocation = MyCharacter->GetActorLocation();
        SetActorLocation(CharacterLocation);  // 힐 이펙트를 캐릭터 위치로 이동
    }
}

void AGnuHealActor::Heal()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        AController* OwnerController = OwnerCharacter->Controller;
        if (OwnerController)
        {
            UGameplayStatics::ApplyDamage(OwnerController->GetPawn(), Damage, OwnerController, this, UDamageType::StaticClass());
        }
    }
}

void AGnuHealActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGnuHealActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AGnuHealActor::DestroyActor()
{
    // 타이머 중지
    GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
    GetWorld()->GetTimerManager().ClearTimer(HealTimer);

    // 소멸 이펙트 재생
    NiagaraComponent->SetAutoDestroy(true); // 소멸 후 자동 삭제 설정
    NiagaraComponent->Deactivate();        // 이펙트 비활성화

    Destroy();
}