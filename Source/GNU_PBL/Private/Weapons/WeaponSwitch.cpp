// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponSwitch.h"
#include "Weapons/Gun.h"

// Sets default values
AWeaponSwitch::AWeaponSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Tags.Add(FName("WeaponSwitch"));
}

TSubclassOf<AGun> AWeaponSwitch::Switching()
{
	return GunClass;
}

// Called when the game starts or when spawned
void AWeaponSwitch::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

