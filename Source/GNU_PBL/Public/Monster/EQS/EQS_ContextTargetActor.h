// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQS_ContextTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UEQS_ContextTargetActor : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	// 블랙보드에서 오브젝트를 가져와 EQS 쿼리에서 사용할 수 있도록 하는 메서드
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
