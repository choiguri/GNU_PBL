// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GnuBaseCharacter.h"
#include "GnuMyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GNU_PBL_API AGnuMyCharacter : public AGnuBaseCharacter
{
	GENERATED_BODY()

public:
	AGnuMyCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	virtual void InitAbilityActorInfo() override;
	
};
