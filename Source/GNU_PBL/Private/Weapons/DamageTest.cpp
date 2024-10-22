// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/DamageTest.h"

// Sets default values
ADamageTest::ADamageTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Tags.Add(FName("Enemy"));
}

void ADamageTest::GetDamage(int power)
{
	HP -= power;
	UE_LOG(LogTemp, Warning, TEXT("HP : %d"), HP);
}

// Called when the game starts or when spawned
void ADamageTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamageTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

