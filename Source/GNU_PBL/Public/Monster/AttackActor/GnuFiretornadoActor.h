// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "GnuFiretornadoActor.generated.h"


class UProjectileMovementComponent;


UCLASS()
class GNU_PBL_API AGnuFiretornadoActor : public AGnuAttackCollisionActor
{
	GENERATED_BODY()
	
public:
    AGnuFiretornadoActor();

    void LaunchProjectile(AActor* IgnoredActor);  // 발사 함수

private:
    FTimerHandle DestructionTimerHandle;    // 타이머 핸들

    void DestroyFiretornado(); // 파이어토네이도 삭제 함수


protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
