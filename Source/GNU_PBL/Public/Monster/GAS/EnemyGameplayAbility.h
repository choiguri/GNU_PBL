// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EnemyGameplayAbility.generated.h"

// 얘가 Worrior Gameplay Ability 입니다.


// 전역 부분
class UPawnCombatComponent;
class UEnemyAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEnemyAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};
/**
 * 
 */
UCLASS()
class GNU_PBL_API UEnemyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()


protected:
	// 전역 부분
	//~ Begin UGameplayAbility Interface.
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface.


	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Ability")
	EEnemyAbilityActivationPolicy AbilityActivationPolicy = EEnemyAbilityActivationPolicy::OnTriggered;

	UFUNCTION(BlueprintPure, Category = "Enemy|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Enemy|Ability")
	UEnemyAbilitySystemComponent* GetEnemyAbilitySystemComponentFromActorInfo() const;
};
