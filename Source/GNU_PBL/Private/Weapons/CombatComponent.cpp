// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CombatComponent.h"
#include "Weapons/GnuWeapon.h"
#include "Characters/GnuMyCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"


UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}



void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();


	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UCombatComponent::EquipWeapon(AGnuWeapon* WeaponToEquip)
{
	if (GnuCharacter == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = GnuCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, GnuCharacter->GetMesh());
	}

	EquippedWeapon->SetOwner(GnuCharacter);

	
}

