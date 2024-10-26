// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GAS/GnuEnemyGameplayAbility.h"
#include "Monster/GnuEnemyCharacter.h"

AGnuEnemyCharacter* UGnuEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedGnuEnemyCharacter.IsValid())
	{
		CachedGnuEnemyCharacter = Cast<AGnuEnemyCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedGnuEnemyCharacter.IsValid() ? CachedGnuEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UGnuEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();;
}
