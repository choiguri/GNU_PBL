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

    //이 발사체가 가할 대미지 타입과 대미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    TSubclassOf<class UDamageType> DamageType;

    //이 발사체가 가하는 대미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    float Damage;

private:
    FTimerHandle DestructionTimerHandle; // 파괴 타이머 핸들

    void DestroyFireball(); // 파이어볼 삭제 함수

protected:
    virtual void BeginPlay() override;

    UFUNCTION(Category = "Projectile")
    void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};