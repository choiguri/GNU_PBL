// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/GnuWeapon.h"
#include "GnuProjectileWeapon.generated.h"

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	EPT_Bullet UMETA(DisplayName = "Bullet"),
	EPT_Razer UMETA(DisplayName = "Razer"),
	EPT_Grenade UMETA(DisplayName = "Grenade")
};

UCLASS()
class GNU_PBL_API AGnuProjectileWeapon : public AGnuWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

	void SetProjectileType(EProjectileType SetType);

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	//TSubclassOf<AActor> BP_Grenade;

	void Boom();

private:
	UPROPERTY(EditAnyWhere)
	TSubclassOf<class AGnuProjectile> ProjectileClass;

	UPROPERTY(EditAnyWhere)
	TSubclassOf<class AGnuProjectile> RazerSkillClass;

	UPROPERTY(EditAnyWhere)
	TSubclassOf<class AGnuProjectile> GrenadeSkillClass;


	UPROPERTY(EditAnyWhere)
	TSubclassOf<class AGnuProjectile> SeletedClass;

	UPROPERTY(ReplicatedUsing = OnRep_ProjectileType)
	EProjectileType ProjectileType = EProjectileType::EPT_Bullet;

	void SetClass();

	UFUNCTION()
	void OnRep_ProjectileType();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetProjectileType(EProjectileType SetType);


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
