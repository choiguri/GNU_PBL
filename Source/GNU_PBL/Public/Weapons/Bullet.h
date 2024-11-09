// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Components/SphereComponent.h"
#include "Bullet.generated.h"

UCLASS()
class GNU_PBL_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	void Die();

	void SetPower(float DamageAmount);

	void SetDirection(const FVector& Direction);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* movementComp;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* TrailParticleSystem;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* ImpactParticle;

	float Power = 10.f;
};
