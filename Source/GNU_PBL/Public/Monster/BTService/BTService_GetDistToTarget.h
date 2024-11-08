// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_GetDistToTarget.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTService_GetDistToTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_GetDistToTarget();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
