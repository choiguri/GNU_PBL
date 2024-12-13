// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GnuProjectile.generated.h"

UCLASS()
class GNU_PBL_API AGnuProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AGnuProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* ChargeNiagaraComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* GrenadeBeforeNiagara;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* GrenadeCastNiagaraComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* GrenadeCastNiagara;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* GrenadepRojectileNiagara;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* GrenadeBoomNiagara;

	UPROPERTY(EditAnywhere)
	class USoundWave* GrenadeBoomSound;

	UPROPERTY(EditAnywhere)
	class USoundWave* GrenadeCastSound;

	UPROPERTY(EditAnywhere)
	class USoundWave* GrenadeProjectileSound;

private:

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;


	UPROPERTY(EditAnywhere)
	class UParticleSystem* TracerParticle;

	UPROPERTY()
	class UParticleSystemComponent* TracerParticleComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	// 사운드
	UPROPERTY(EditAnywhere)
	class USoundCue* ShotSound;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	class USoundWave* waveShotSound;

	// 스킬 이펙트는 NiagaraSystem이라 기존에 있던 UParticleSystem말고 UNiagaraSystem 새로 멤버변수 만듦
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* TracerNiagaraComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* MuzzleNiagara;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ImpactNiagara;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TracerNiagara;
	
public:	

};
