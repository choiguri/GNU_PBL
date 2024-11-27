// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "GnuGroundSpikeCollisionActor.generated.h"


class UProjectileMovementComponent;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuGroundSpikeCollisionActor : public AGnuAttackCollisionActor
{
	GENERATED_BODY()
	

public:
    AGnuGroundSpikeCollisionActor();

    void LaunchProjectile(AActor* IgnoredActor);  // 발사 함수

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

private:
    FTimerHandle DestructionTimerHandle; // 파괴 타이머 핸들

    void DestroyGroundSpikeCollision(); // 파이어볼 삭제 함수

protected:
    virtual void BeginPlay() override;

    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    //virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    //    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
