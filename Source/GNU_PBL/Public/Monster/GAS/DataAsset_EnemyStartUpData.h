// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/GAS/DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartUpData.generated.h"



class UGnuEnemyGameplayAbility;

/**
 * 
 */
UCLASS()
class GNU_PBL_API UDataAsset_EnemyStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()
	
public:
	virtual void GiveToAbilitySystemComponent(UEnemyAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

private:

	// 적의 어빌리티만 채울 수 있음 level, Surce, 등등
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UGnuEnemyGameplayAbility > > EnemyCombatAbilities;
};
