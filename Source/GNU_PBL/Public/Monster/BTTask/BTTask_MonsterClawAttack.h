// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterClawAttack.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_MonsterClawAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MonsterClawAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

};
