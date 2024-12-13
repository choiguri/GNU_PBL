// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/GnuWeapon.h"
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

	void ReloadFinished(bool bPressed);

	bool bFireButtonPressed;

	bool bReloadButtonPressed;

	FORCEINLINE FVector GetHitTarget() { return HitTarget; }

protected:

	virtual void BeginPlay() override;

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void ReloadButtonPressed(bool bPressed);


	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	UPROPERTY()
	class AGnuMyCharacter* GnuCharacter;
	UPROPERTY()
	class AGnuMyPlayerController* Controller;
	UPROPERTY()
	class AGNUHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AGnuWeapon* EquippedWeapon;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairInFireFactor;

	FVector HitTarget;

	FTimerHandle FireTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireDelay = 0;
	
	void StartFireTimer();
	void FireTimerFinished();

	// ���߿� �ڵ��� �ƴ� ������ ���� false�̸� �ܹ߷�
	// �Ƹ� GnuWeapon.h �� �Űܾ� ��
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic = true;

	bool bCanFire = true;

	bool CanFire();
};
