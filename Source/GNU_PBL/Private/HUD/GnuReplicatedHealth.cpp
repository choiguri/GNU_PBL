// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GnuReplicatedHealth.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


void UGnuReplicatedHealth::SetHealthText(FString TextToDisplay)
{
	if (ReplicatedHealthText)
	{
		ReplicatedHealthText->SetText(FText::FromString(TextToDisplay));
	}
}

void UGnuReplicatedHealth::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}
