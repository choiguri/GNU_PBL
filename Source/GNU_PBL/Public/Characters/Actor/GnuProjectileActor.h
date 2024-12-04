#pragma once

#include "CoreMinimal.h"
#include "Characters/Actor/GnuActorCollisionBase.h"
#include "NiagaraComponent.h"
#include "NiagaraTypes.h"
#include "GnuProjectileActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;

UCLASS()
class GNU_PBL_API AGnuProjectileActor : public AGnuActorCollisionBase
{
    GENERATED_BODY()

public:
    AGnuProjectileActor();

    void LaunchProjectile(AActor* IgnoredActor);  // 발사 함수

private:
    float Damage;

    FTimerHandle TimerHandle; // 타이머 핸들

    FVector PreviousLocation; // 나이아가라 이펙트의 이전 위치

    void DestroyActor(); // 엑터 삭제 함수


protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* NiagaraComponent;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* MuzzleComponent;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* TargetComponent;

    UPROPERTY(VisibleAnywhere)
    UNiagaraComponent* FlyComponent;

    UPROPERTY(EditAnywhere)
    UNiagaraSystem* NiagaraSystem;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UProjectileMovementComponent* ProjectileMovementComp;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* ProjectileBoxComponent;

    // 충돌 시 호출되는 함수
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
