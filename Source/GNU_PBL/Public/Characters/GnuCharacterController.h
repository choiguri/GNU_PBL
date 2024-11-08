// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "GnuCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuCharacterController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	

public:
	AGnuCharacterController();


	//~ Begin IGenericTeamAgentInterface Interface.
	virtual FGenericTeamId GetGenericTeamId() const;
	//~ End IGenericTeamAgentInterface Interface


private:
	FGenericTeamId CharacterTeamID;

};
