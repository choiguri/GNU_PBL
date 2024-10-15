// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class GNU_PBL_API AGun : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PullTrigger();
	virtual void ReleaseTrigger();
	virtual void Reload();

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;

	UPROPERTY(EditAnywhere)
	int MaxAmmo = 30;

	UPROPERTY(EditAnywhere)
	int RemainAmmo = MaxAmmo;
	UPROPERTY(EditAnywhere)
	float ReroadingDelay = 3.f;
	UPROPERTY(EditAnywhere)
	float FireInterval = 0.1f;

	FTimerHandle FireTimerHandle;  // 타이머 핸들 변수 추가

	void Fire();  // 발사 기능 구현 함수

};
