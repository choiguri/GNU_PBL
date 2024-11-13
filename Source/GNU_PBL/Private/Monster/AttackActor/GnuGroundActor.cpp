// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuGroundActor.h"
#include "NiagaraComponent.h"

AGnuGroundActor::AGnuGroundActor()
{
	PrimaryActorTick.bCanEverTick = true;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(BoxComponent);
}


void AGnuGroundActor::BeginPlay()
{
    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentHit.AddDynamic(this, &AGnuGroundActor::OnHit);
    }

    // 초기화 시 파티클 활성화
    if (NiagaraComponent)
    {
        NiagaraComponent->Activate();
    }

    // 브레스는 따로 삭제 함수 구현할 것
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuGroundActor::DestroyGround, 3.0f, false);
}

void AGnuGroundActor::DestroyGround()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate(); // 파티클 시스템 비활성화
    }

    Destroy(); // 액터 삭제
}

void AGnuGroundActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때 파이어볼 삭제
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Overlap with: ") + OtherActor->GetName());
    }
}