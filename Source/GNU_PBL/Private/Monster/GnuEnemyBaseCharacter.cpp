// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuEnemyBaseCharacter.h"
#include "Monster/GAS/EnemyAbilitySystemComponent.h"
#include "Monster/GAS/EnemyAttributeSet.h"

// Sets default values
AGnuEnemyBaseCharacter::AGnuEnemyBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetMesh()->bReceivesDecals = false;
	EnemyAbilitySystemComponent = CreateDefaultSubobject<UEnemyAbilitySystemComponent>(TEXT("EnemyAbilitySystemComponent"));
	EnemyAttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
}

UAbilitySystemComponent* AGnuEnemyBaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void AGnuEnemyBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (EnemyAbilitySystemComponent)
	{
		EnemyAbilitySystemComponent->InitAbilityActorInfo(this, this);

		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assign start up data to %s"), *GetName());
	}
}
