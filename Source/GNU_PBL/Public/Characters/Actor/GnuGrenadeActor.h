// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Actor/GnuActorCollisionBase.h"
#include "GnuGrenadeActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USphereComponent;

UCLASS()
class GNU_PBL_API AGnuGrenadeActor : public AGnuActorCollisionBase
{
	GENERATED_BODY()
public:
    AGnuGrenadeActor();
    void LaunchProjectile(AActor* IgnoredActor, FVector SpawnLocation, FRotator SpawnRotation);  // 발사 함수

private:
    FTimerHandle DestroyTimer; // 파괴 타이머 핸들
    FTimerHandle GrenadeTimer;
    float Damage;
    void SpawnEMPVortex(AActor* IgnoredActor, FVector SpawnLocation, FRotator SpawnRotation);
    void DestroyActor(); // 엑터 삭제 함수

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    //UPROPERTY(EditAnywhere)
    //USphereComponent* SphereComponent;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem* MuzzleNiagaraSystem;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* NiagaraComponent;

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
