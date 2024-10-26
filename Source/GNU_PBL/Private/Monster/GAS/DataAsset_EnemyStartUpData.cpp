// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GAS/DataAsset_EnemyStartUpData.h"
#include "Monster/GAS/EnemyAbilitySystemComponent.h"
#include "Monster/GAS/GnuEnemyGameplayAbility.h"


void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UEnemyAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf < UGnuEnemyGameplayAbility >& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();	// 아바타 가져오는 함수
			AbilitySpec.Level = ApplyLevel;								// 레벨 가져오는 함수
			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
