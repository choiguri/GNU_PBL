// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EQSTestingPawn.h"
#include "GenericTeamAgentInterface.h"
#include "EQS_TestingPawnBase.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AEQS_TestingPawnBase : public AEQSTestingPawn, public IGenericTeamAgentInterface
{
	GENERATED_BODY()


public:
	AEQS_TestingPawnBase();


	//~ Begin IGenericTeamAgentInterface Interface.
	virtual FGenericTeamId GetGenericTeamId() const;
	//~ End IGenericTeamAgentInterface Interface
private:
	FGenericTeamId EQSTeamID;
	
};
