// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Actor/GnuActorCollisionBase.h"
#include "GnuGrenadeActor.generated.h"
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class GNU_PBL_API AGnuGrenadeActor : public AGnuActorCollisionBase
{
	GENERATED_BODY()
public:
    AGnuGrenadeActor();
    void LaunchProjectile(AActor* IgnoredActor);  // 발사 함수

private:
    FTimerHandle DestructionTimerHandle; // 파괴 타이머 핸들

    // 타이머 핸들
    FTimerHandle TimerHandle;
    void SpawnEMPVortex(AActor* IgnoredActor);
    void DestroyActor(); // 엑터 삭제 함수

    FVector PreviousLocation;

    float Damage;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem* FlyingNiagaraSystem;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem* MuzzleNiagaraSystem;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem* TargetNiagaraSystem;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* NiagaraComponent;

    FVector ProjectileOffset;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootSceneComponent;

    // 스폰할 액터 클래스 (BP_Projectile_EMP_Vortex)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
    TSubclassOf<AActor> BP_Projectile_EMP_Vortex;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
