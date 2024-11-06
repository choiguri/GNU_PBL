// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EQS_ContextTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UEQS_ContextTargetActor : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	// ProvideSingleActor 함수를 통해 TargetActor를 반환
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
