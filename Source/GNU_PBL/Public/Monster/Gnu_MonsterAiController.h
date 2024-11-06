// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Gnu_MonsterAiController.generated.h"


class UAIPerceptionComponent;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnu_MonsterAiController : public AAIController
{
	GENERATED_BODY()
public:
	AGnu_MonsterAiController(const FObjectInitializer& ObjectInitializer);


	virtual void Tick(float DeltaSecond) override;

protected:
	virtual void BeginPlay() override;


	// GAS 따라 한 코드
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;

	UFUNCTION()
	void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);


private:
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* AIBehavior;
};