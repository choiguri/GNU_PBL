// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Actor/GnuActorCollisionBase.h"
#include "GnuProjectileActor.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;

UCLASS()
class GNU_PBL_API AGnuProjectileActor : public AGnuActorCollisionBase
{
    GENERATED_BODY()

public:
    AGnuProjectileActor();

    void LaunchProjectile(AActor* IgnoredActor);  // 발사 함수

private:
    FTimerHandle DestructionTimerHandle; // 파괴 타이머 핸들

    void DestroyFireball(); // 파이어볼 삭제 함수

    FVector PreviousLocation;

    float Damage;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* NiagaraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovementComponent;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};