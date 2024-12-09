// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuGroundSpikeActor.h"
#include "NiagaraComponent.h" // UNiagaraComponent 헤더 추가
#include "NiagaraFunctionLibrary.h" // Niagara 기능을 위한 헤더
#include "Kismet/GameplayStatics.h"


AGnuGroundSpikeActor::AGnuGroundSpikeActor()
{
    // NiagaraComponent 초기화 및 설정
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);
    //NiagaraComponent->SetIsReplicated(true);  // 네트워크에서 복제 활성화

    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    }
}

void AGnuGroundSpikeActor::BeginPlay()
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
    }

    GetWorld()->GetTimerManager().SetTimer(DestroyActorTimerHandle, this, &AGnuGroundSpikeActor::DestroyGroundSpike, 3.0f, false); // 10초 뒤에 삭제
}


void AGnuGroundSpikeActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}


void AGnuGroundSpikeActor::DestroyGroundSpike()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate(); // 새로운 파티클 생성을 중단
        NiagaraComponent->SetAutoDestroy(true); // 파티클이 모두 끝나면 컴포넌트 삭제
    }

    // 파티클 소멸 후 액터 삭제를 위한 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestroyActorTimerHandle, this, &AGnuGroundSpikeActor::DestroyActor, 2.0f, false); // 2초 지연
}

void AGnuGroundSpikeActor::DestroyActor()
{
    Destroy();
}