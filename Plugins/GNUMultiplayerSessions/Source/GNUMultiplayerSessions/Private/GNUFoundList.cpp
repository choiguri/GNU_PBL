// Fill out your copyright notice in the Description page of Project Settings.


#include "GNUFoundList.h"
#include "Components/TextBlock.h"
#include "OnlineSessionSettings.h"

void UGNUFoundList::SetDisplayText(UTextBlock* TextBlock, FString TextToDisplay)
{
	if (TextBlock)
	{
		TextBlock->SetText(FText::FromString(TextToDisplay));
	}
}

void UGNUFoundList::OnJoinButtonClicked()
{
	if (OnJoinButtonClickedFunc)
	{
		OnJoinButtonClickedFunc();
	}
}