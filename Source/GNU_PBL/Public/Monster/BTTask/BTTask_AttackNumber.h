// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackNumber.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_AttackNumber : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_AttackNumber();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	FTimerHandle InitNumber;
};
