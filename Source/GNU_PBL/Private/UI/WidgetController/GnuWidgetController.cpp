// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/GnuWidgetController.h"

void UGnuWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UGnuWidgetController::BroadcastInitialValues()
{

}

void UGnuWidgetController::BindCallbacksToDependencies()
{

}