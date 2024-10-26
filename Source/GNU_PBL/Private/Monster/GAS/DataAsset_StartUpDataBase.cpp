// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GAS/DataAsset_StartUpDataBase.h"
#include "Monster/GAS/EnemyAbilitySystemComponent.h"
#include "Monster/GAS/EnemyGameplayAbility.h"


void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UEnemyAbilitySystemComponent* InEnemyASCToGive, int32 ApplyLevel)
{
	check(InEnemyASCToGive);

	GrantAbilities(ActivateOnGivenAbilities, InEnemyASCToGive, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InEnemyASCToGive, ApplyLevel);
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UEnemyGameplayAbility>>& InAbilitiesToGive, UEnemyAbilitySystemComponent* InEnemyASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UEnemyGameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InEnemyASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InEnemyASCToGive->GiveAbility(AbilitySpec);
	}
}
