// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Actor/GnuActorCollisionBase.h"
#include "GnuHealActor.generated.h"

class UNiagaraComponent;

UCLASS()
class GNU_PBL_API AGnuHealActor : public AGnuActorCollisionBase
{
	GENERATED_BODY()
	
public:
	AGnuHealActor();

private:
    FTimerHandle DestroyTimer; // 파괴 타이머 핸들
    FTimerHandle HealTimer;
    FVector PreviousLocation;
    float Damage;

    UPROPERTY(EditAnywhere)
    class USoundWave* waveHealSound;

    void DestroyActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void Heal();

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* NiagaraComponent;

    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
