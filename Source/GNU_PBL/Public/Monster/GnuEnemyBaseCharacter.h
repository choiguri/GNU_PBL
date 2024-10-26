// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GnuEnemyBaseCharacter.generated.h"

class UEnemyCombatComponent;
class UEnemyAbilitySystemComponent;
class UEnemyAttributeSet;
class UDataAsset_StartUpDataBase;

UCLASS()
class GNU_PBL_API AGnuEnemyBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGnuEnemyBaseCharacter();


	//~ Begin IAbilitySystemInterface Interface.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	//~ End IAbilitySystemInterface Interface

protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UEnemyAbilitySystemComponent* EnemyAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UEnemyAttributeSet* EnemyAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

public:
	FORCEINLINE UEnemyAbilitySystemComponent* GetWarriorAbilitySystemComponent() const { return EnemyAbilitySystemComponent; }
	FORCEINLINE UEnemyAttributeSet* GetEnemyAttributeSet() const { return EnemyAttributeSet; }

};
