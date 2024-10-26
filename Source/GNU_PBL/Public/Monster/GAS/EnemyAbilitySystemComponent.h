// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Monster/GnuTypes/GnuStuctTypes.h"
#include "EnemyAbilitySystemComponent.generated.h"


// WarriorAbilitySystemComponent ¿ªÇÒ

/**
 * 
 */
UCLASS()
class GNU_PBL_API UEnemyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	/*UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);*/

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemovedGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
};
