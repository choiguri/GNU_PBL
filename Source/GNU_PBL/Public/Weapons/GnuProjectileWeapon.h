// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/GnuWeapon.h"
#include "GnuProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuProjectileWeapon : public AGnuWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnyWhere)
	TSubclassOf<class AProjectile> ProjectileClass;
};
