// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "Monster/GnuMonster.h"
#include "GnuGroundActor.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class AGnuMonster;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuGroundActor : public AGnuAttackCollisionActor
{
	GENERATED_BODY()
	
public:
    AGnuGroundActor();

    UFUNCTION()
    void DestroyGround(); // 파이어브레스 삭제 함수
    void DestroyBox();

    void LaunchProjectile(AActor* IgnoredActor, FVector* Location, FRotator* Rotation);  // 발사 함수

    // 추가된 BoxComponent (발사체로 사용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* ProjectileBoxComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement;

    virtual float GetDamage() const override { return 25.0f; }


    AGnuMonster* Monster;

private:
    FTimerHandle DestructionTimerHandle;    // 타이머 핸들
    FTimerHandle DestructionTimerHandle2;    // 타이머 핸들

    FVector InitialLocation; // 발사체의 초기 위치
    float MaxDistance = 1500.0f; // 발사체가 이동 가능한 최대 거리

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime);


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
    UNiagaraComponent* NiagaraComponent;


    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
