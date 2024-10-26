// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterFirebreathAttack.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_MonsterFirebreathAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MonsterFirebreathAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	
};
