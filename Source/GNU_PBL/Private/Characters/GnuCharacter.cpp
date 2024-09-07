// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuCharacter.h"

// Sets default values
AGnuCharacter::AGnuCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGnuCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGnuCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGnuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

