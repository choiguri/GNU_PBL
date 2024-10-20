// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystems/GnuAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h" // SpringArm
#include "Camera/CameraComponent.h" // TPP, FPP Camera

AGnuBaseCharacter::AGnuBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AGnuBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AGnuBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGnuBaseCharacter::InitAbilityActorInfo()
{
}

