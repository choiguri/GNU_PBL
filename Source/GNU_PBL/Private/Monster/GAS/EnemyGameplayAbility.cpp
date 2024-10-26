// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GAS/EnemyGameplayAbility.h"
#include "Monster/GAS/EnemyAbilitySystemComponent.h"
#include "Monster/GAS/PawnCombatComponent.h"



void UEnemyGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EEnemyAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UEnemyGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EEnemyAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

UPawnCombatComponent* UEnemyGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}


UEnemyAbilitySystemComponent* UEnemyGameplayAbility::GetEnemyAbilitySystemComponentFromActorInfo() const
{
	return Cast<UEnemyAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}


