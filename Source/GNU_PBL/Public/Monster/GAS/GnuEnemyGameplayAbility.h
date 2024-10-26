// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/GAS/EnemyGameplayAbility.h"
#include "GnuEnemyGameplayAbility.generated.h"

// Warrior Enemy Gameplay Ability;


class AGnuEnemyCharacter;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class GNU_PBL_API UGnuEnemyGameplayAbility : public UEnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Enemy|Ability")
	AGnuEnemyCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Enemy|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

private:
	TWeakObjectPtr<AGnuEnemyCharacter> CachedGnuEnemyCharacter;
};
