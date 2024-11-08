// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GnuBaseCharacter.generated.h"

UCLASS()
class GNU_PBL_API AGnuBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGnuBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
