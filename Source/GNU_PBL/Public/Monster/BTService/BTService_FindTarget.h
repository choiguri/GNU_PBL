// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FindTarget.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTService_FindTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_FindTarget();


protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
