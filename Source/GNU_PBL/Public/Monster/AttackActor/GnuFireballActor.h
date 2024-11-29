// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "GnuFireballActor.generated.h"

class UProjectileMovementComponent;


UCLASS()
class GNU_PBL_API AGnuFireballActor : public AGnuAttackCollisionActor
{
    GENERATED_BODY()

public:
    AGnuFireballActor();

    void LaunchProjectile(AActor* IgnoredActor);  // 발사 함수

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

    // 네트워크 복제 설정
    UPROPERTY(Replicated)
    FVector ProjectileVelocity;

    UPROPERTY(Replicated)
    AActor* OwnerActor;

    virtual float GetDamage() const override { return 10.0f; }

private:
    FTimerHandle DestructionTimerHandle; // 파괴 타이머 핸들

    UFUNCTION()
    void DestroyFireball(); // 파이어볼 삭제 함수

protected:
    virtual void BeginPlay() override;

    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};