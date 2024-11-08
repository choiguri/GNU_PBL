// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AttackActor/GnuFirebreathActor.h"
#include "NiagaraComponent.h" // UNiagaraComponent 헤더 추가
#include "NiagaraFunctionLibrary.h" // Niagara 기능을 위한 헤더

AGnuFirebreathActor::AGnuFirebreathActor()
{
	PrimaryActorTick.bCanEverTick = true;

    // NiagaraComponent 초기화 및 설정
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(BoxComponent);


    // 부모 클래스에서 상속받은 BoxComponent를 초기화
    if (BoxComponent)
    {
        BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxComponent->SetCollisionResponseToAllChannels(ECR_Block); // 모든 채널에 대해 충돌 허용
        BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 캐릭터에 대해 오버랩 허용
    }
}


void AGnuFirebreathActor::DestroyFirebreath()
{
    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate(); // 파티클 시스템 비활성화
    }

    Destroy(); // 액터 삭제
}

void AGnuFirebreathActor::BeginPlay()
{

    Super::BeginPlay();


    // 충돌을 위해 이벤트 바인딩
    if (BoxComponent)
    {
        BoxComponent->OnComponentHit.AddDynamic(this, &AGnuFirebreathActor::OnHit);
    }

    // 초기화 시 파티클 활성화
    if (NiagaraComponent)
    {
        NiagaraComponent->Activate();
    }

    // 브레스는 따로 삭제 함수 구현할 것
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuFirebreathActor::DestroyFirebreath, 3.0f, false);
}

void AGnuFirebreathActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && (OtherActor != this))
    {
        // 충돌한 액터가 벽이나 캐릭터일 때 파이어브레스 삭제
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Overlap with: ") + OtherActor->GetName());
        DestroyFirebreath();
    }
}
