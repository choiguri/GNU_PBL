// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterFlyingAttack.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_MonsterFlyingAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MonsterFlyingAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	
};
