// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "GnuGroundSpikeActor.generated.h"


class UNiagaraComponent;
class UProjectileMovementComponent;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuGroundSpikeActor : public AGnuAttackCollisionActor
{
	GENERATED_BODY()


public:
    AGnuGroundSpikeActor();

    void DestroyGroundSpike(); // 삭제 함수

private:
    FTimerHandle DestructionTimerHandle;    // 타이머 핸들

    FTimerHandle DestroyActorTimerHandle;

    void DestroyActor();

protected:
    virtual void BeginPlay() override;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
    UNiagaraComponent* NiagaraComponent;

    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
