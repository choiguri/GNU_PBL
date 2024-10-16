// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuCharacterPlayerState.h"
#include "AbilitySystems/GnuAbilitySystemComponent.h"
#include "AbilitySystems/GnuAttributeSet.h"

AGnuCharacterPlayerState::AGnuCharacterPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGnuAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGnuAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* AGnuCharacterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
