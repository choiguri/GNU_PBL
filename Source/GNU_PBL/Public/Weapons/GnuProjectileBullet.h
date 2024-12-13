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
	
private:
	FTimerHandle GrenadeBoomTimerHandle;  // 고유 타이머 핸들 생성
	FTimerHandle DestoryTimer;

	void ApplyDamageInRange(AController* ownerController);
	void CastGrenade(AController* ownerController, UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void BoomGrenade(AController* ownerController, FVector HitLocation, FRotator HitRotation);
	void DestoryGrenade(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DestroyChargeNiagaraComponent();

	// 멀티캐스트로 이펙트 스폰
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CastGrenadeEffect(FVector HitLocation, FRotator HitRotation);

	// 멀티캐스트로 이펙트 스폰
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BoomGrenadeEffect(AController* ownerController, FVector HitLocation, FRotator HitRotation);

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
