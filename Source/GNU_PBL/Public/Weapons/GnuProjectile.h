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
	class USoundCue* ImpactSound;



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
