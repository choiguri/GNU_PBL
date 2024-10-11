// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Gnu_MonsterAiController.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnu_MonsterAiController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSecond) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
};
