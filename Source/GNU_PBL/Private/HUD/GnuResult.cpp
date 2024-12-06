// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GnuResult.h"
#include "Components/TextBlock.h"

void UGnuResult::SetVictoryText()
{
	if (ResultText)
	{
		ResultText->SetText(FText::FromString("Victory"));
	}
}

void UGnuResult::SetDefeatText()
{
	if (ResultText)
	{
		ResultText->SetText(FText::FromString("Defeated"));
	}
}
