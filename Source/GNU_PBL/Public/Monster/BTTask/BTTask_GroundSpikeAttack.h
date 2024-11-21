// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GroundSpikeAttack.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_GroundSpikeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GroundSpikeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
