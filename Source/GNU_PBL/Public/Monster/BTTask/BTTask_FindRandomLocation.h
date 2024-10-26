// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	// explicit : 특정 생성장가 암시적 변환으로 호출되지 않도록 명시적으로 호출해야 한다는 의미
	// 주로 타입 변환으로 인한 예기치 않은 동작을 방지하기 위해 사용됨
	// explicit UBTTask_FindRandocmLocation(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
