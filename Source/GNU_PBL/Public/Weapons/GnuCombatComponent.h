// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GnuCombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GNU_PBL_API UGnuCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGnuCombatComponent();
	friend class AGnuMyCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void EquipWeapon(class AGnuWeapon* WeaponToEquip);

	void ReloadFinished();

protected:

	virtual void BeginPlay() override;

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void ReloadButtonPressed();


	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	class AGnuMyCharacter* GnuCharacter;
	class AGnuMyPlayerController* Controller;
	class AGNUHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AGnuWeapon* EquippedWeapon;
	
	bool bFireButtonPressed;

	bool bReloadButtonPressed;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairInFireFactor;

	FVector HitTarget;

	FTimerHandle FireTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireDelay = .15f;
	
	void StartFireTimer();
	void FireTimerFinished();

	// 나중에 자동이 아닌 웨폰을 위해 false이면 단발로
	// 아마 GnuWeapon.h 로 옮겨야 함
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic = true;

	bool bCanFire = true;

	bool CanFire();
};
