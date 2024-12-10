// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GnuWeapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class GNU_PBL_API AGnuWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AGnuWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);
	USkeletalMeshComponent* GetMesh(); // 11.26
	virtual void Fire(const FVector& HitTarget);

	void UpdateAmmo();

	virtual void OnRep_Owner() override;

	void Dropped();

	// Crosshair 
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairTop;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairBottom;


	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float RPM = 400.f;

	float Delay = .15f;


protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedCompoonent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedCompoonent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* ReloadAnimation;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo = 30;

	UPROPERTY(EditAnywhere)
	int32 MaxAmmo = 30;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendAmmo();

	UPROPERTY()
	class AGnuMyCharacter* GnuOwnerCharacter;
	UPROPERTY()
	class AGnuMyPlayerController* GnuOwnerController;
	
	void SetRecoil(float DeltaTime);

	float Pitch = 0;
	float Yaw = 0;

	UPROPERTY(ReplicatedUsing = OnRep_InitialTransform)
	FVector InitialRelativeLocation = FVector::ZeroVector;
	UPROPERTY(ReplicatedUsing = OnRep_InitialTransform)
	FRotator InitialRelativeRotation = FRotator::ZeroRotator;

	UFUNCTION()
	void OnRep_InitialTransform();

public:	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; };
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; };

	void Reload();

	bool IsEmptyAmmo();

	void ReloadFinished();
};
