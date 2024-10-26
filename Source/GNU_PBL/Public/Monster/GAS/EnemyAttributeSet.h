// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Monster/GAS/EnemyAbilitySystemComponent.h"
#include "EnemyAttributeSet.generated.h"


// AttributeSet.h 파일에 있는 속성 정의된 값들 가져오기
// 컨트롤 + F 로 helper Function 이라고 검색해서 찾은 주석
// 매크로 만드는데 도움되는 함수들 정의
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class GNU_PBL_API UEnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UEnemyAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;

	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, CurrentHealth)
	UPROPERTY(BlueprintReadOnly, Category = "Health")

	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MaxHealth)
	UPROPERTY(BlueprintReadOnly, Category = "Rage")

	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, CurrentRage)
	UPROPERTY(BlueprintReadOnly, Category = "Rage")

	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MaxRage)
	UPROPERTY(BlueprintReadOnly, Category = "Damage")

	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, AttackPower)
	UPROPERTY(BlueprintReadOnly, Category = "Damage")

	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, DefensePower)
};
