// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/GnuProjectile.h"
#include "GnuProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuProjectileBullet : public AGnuProjectile
{
	GENERATED_BODY()
	
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
