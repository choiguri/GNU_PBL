// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GnuBaseCharacter.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
class UCameraComponent;
UCLASS(Abstract)

class GNU_PBL_API AGnuBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGnuBaseCharacter();

	// IAbilitySystemInterface의 GetAbilitySystemComponent 메서드를 구현합니다.
	// 이 메서드는 캐릭터의 어빌리티 시스템 컴포넌트에 접근할 수 있게 해줍니다.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// AttributeSet의 Getter 메서드입니다. AttributeSet은 캐릭터의 스탯(체력, 스태미나 등)을 포함합니다.
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;
	

	// 무기를 나타내는 SkeletalMeshComponent를 설정합니다. (에디터에서 수정 가능)
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	// 캐릭터의 어빌리티 시스템 컴포넌트를 나타내는 포인터입니다.
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 캐릭터의 속성 세트를 나타내는 포인터입니다.
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


	// 어빌리티 액터 정보를 초기화하는 메서드입니다.
	virtual void InitAbilityActorInfo();


};
