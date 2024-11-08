// Copyright Epic Games, Inc. All Rights Reserved.

#include "GNU_PBLGameMode.h"
#include "UObject/ConstructorHelpers.h"

AGNU_PBLGameMode::AGNU_PBLGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
