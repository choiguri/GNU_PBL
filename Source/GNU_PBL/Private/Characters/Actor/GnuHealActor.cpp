// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Actor/GnuHealActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/GnuMyCharacter.h"
#include "TimerManager.h"  // 타이머 관련 클래스 포함

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

    Damage = 3.0f;
}


void AGnuHealActor::BeginPlay()
{
    Super::BeginPlay();

    // 일정 시간마다 체력을 회복하도록 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DamageHandle, this, &AGnuHealActor::HealOverTime, 3.0f, true);

    // 5초 후에 힐 이펙트를 삭제하도록 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AGnuHealActor::DestroyHeal, 10.0f, false);
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("HealNiagara Begin!!")));
}

void AGnuHealActor::Tick(float DeltaTime)
{
    // Niagara 이펙트를 계속해서 캐릭터 위치에 맞춰 업데이트 (캐릭터와 함께 움직이도록)
}

void AGnuHealActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Overlap 시 Heal 시작
    if (OtherActor && OtherActor->IsA(AGnuMyCharacter::StaticClass()))
    {
        if (OtherActor)
        {
            SetActorLocation(OtherActor->GetActorLocation());  // 캐릭터와 동일한 위치로 이펙트 이동
        }
        AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(OtherActor);
        if (MyCharacter)
        {
            // 체력 회복 시작
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Healing started!")));
        }
    }
}

void AGnuHealActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // Overlap 끝날 때 Heal 중지
    if (OtherActor && OtherActor->IsA(AGnuMyCharacter::StaticClass()))
    {
        AGnuMyCharacter* MyCharacter = Cast<AGnuMyCharacter>(OtherActor);
        if (MyCharacter)
        {
            // 체력 회복 중지
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Healing stopped!")));
        }
    }
}

void AGnuHealActor::HealOverTime()
{

}

void AGnuHealActor::DestroyHeal()
{
    // 타이머 중지
    GetWorld()->GetTimerManager().ClearTimer(DamageHandle);
    Destroy();
}