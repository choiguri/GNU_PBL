// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CombatComponent.h"
#include "Weapons/GnuWeapon.h"
#include "Characters/GnuMyCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	
	if (bFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		ServerFire(HitResult.ImpactPoint);
	}

}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// viewport (화면) 중앙
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	// 2D-> 3D
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0), 
		CrosshairLocation, 
		CrosshairWorldPosition, 
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// 화면 중앙 3D
		FVector Start = CrosshairWorldDirection;

		// Start 지점을 십자방향으로 밀어내기 TRACE_LENGTH =  10000.f
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		
	}
}

void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (GnuCharacter)
	{
		GnuCharacter->PlayFireMontage();
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}



void UCombatComponent::EquipWeapon(AGnuWeapon* WeaponToEquip)
{
	if (GnuCharacter == nullptr || WeaponToEquip == nullptr) return;
	if (EquippedWeapon != nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = GnuCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, GnuCharacter->GetMesh());
	}

	EquippedWeapon->SetOwner(GnuCharacter);

	
}

