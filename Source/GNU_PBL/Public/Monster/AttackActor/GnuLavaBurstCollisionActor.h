// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "GnuLavaBurstCollisionActor.generated.h"


class UProjectileMovementComponent;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuLavaBurstCollisionActor : public AGnuAttackCollisionActor
{
	GENERATED_BODY()
	
public:
    AGnuLavaBurstCollisionActor();

    void LaunchProjectile(AActor* IgnoredActor);  // 발사 함수

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

private:
    FTimerHandle DestructionTimerHandle; // 파괴 타이머 핸들

    void DestroyLavaBurstCollision(); // 삭제 함수

protected:
    virtual void BeginPlay() override;

    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
