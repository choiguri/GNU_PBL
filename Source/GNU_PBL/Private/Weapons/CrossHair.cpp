// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CrossHair.h"
#include "Characters/GnuCharacter.h"
#include <Components/Image.h>

void UCrossHair::SetAimRate(float Aimrate)
{
    
    fTarget_Aimrate = Aimrate;
    Cross_L = Cast<UImage>(GetWidgetFromName(TEXT("Cross_L")));
    Cross_R = Cast<UImage>(GetWidgetFromName(TEXT("Cross_R")));
    Cross_T = Cast<UImage>(GetWidgetFromName(TEXT("Cross_T")));
    Cross_B = Cast<UImage>(GetWidgetFromName(TEXT("Cross_B")));


    Cross_L->SetRenderTranslation(FVector2D(-16.f * fTarget_Aimrate, 0.f));
    Cross_R->SetRenderTranslation(FVector2D(-16.f * fTarget_Aimrate, 0.f));
    Cross_T->SetRenderTranslation(FVector2D(-16.f * fTarget_Aimrate, 0.f));
    Cross_B->SetRenderTranslation(FVector2D(-16.f * fTarget_Aimrate, 0.f));
}

void UCrossHair::BindUserAimRate(AGnuCharacter* GnuCharacter)
{
    if (GnuCharacter == nullptr) return;

    // Bind the delegate function to CrossHair's SetAimRate function
    GnuCharacter->func_Player_Aimrate.BindUObject(this, &UCrossHair::SetAimRate);
}
