// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"


class UEnemyGameplayAbility;
class UEnemyAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class GNU_PBL_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UEnemyAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UEnemyGameplayAbility > > ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UEnemyGameplayAbility > > ReactiveAbilities;

	void GrantAbilities(const TArray< TSubclassOf < UEnemyGameplayAbility > >& InAbilitiesToGive, UEnemyAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
};
