// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MontsterExampleAnim.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_MontsterExampleAnim : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_MontsterExampleAnim();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
