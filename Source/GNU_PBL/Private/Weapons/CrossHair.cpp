// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CrossHair.h"
#include "Characters/GnuCharacter.h"
#include <Components/Image.h>



void UCrossHair::NativeConstruct()
{
    Super::NativeConstruct();

    UICross_L = Cast<UImage>(GetWidgetFromName(TEXT("Cross_L")));
    UICross_R = Cast<UImage>(GetWidgetFromName(TEXT("Cross_R")));
    UICross_T = Cast<UImage>(GetWidgetFromName(TEXT("Cross_T")));
    UICross_B = Cast<UImage>(GetWidgetFromName(TEXT("Cross_B")));
    UICross_C = Cast<UImage>(GetWidgetFromName(TEXT("Cross_C")));

    UpdateCrossHair(1);
}

void UCrossHair::UpdateCrossHair(float aimrate)
{
    if (UICross_L)
    {
        UICross_L->SetRenderTranslation(FVector2D(-8.f * aimrate, 0.f));
    }

    if (UICross_R)
    {
        UICross_R->SetRenderTranslation(FVector2D(8.f * aimrate, 0.f));
    }

    if (UICross_T)
    {
        UICross_T->SetRenderTranslation(FVector2D(0.f, -8.f * aimrate));
    }

    if (UICross_B)
    {
        UICross_B->SetRenderTranslation(FVector2D(0.f, 8.f * aimrate));
    }
}


void UCrossHair::SetAimRate(float Aimrate)
{
    fTarget_Aimrate = Aimrate;
}


void UCrossHair::BindUserAimRate(AGnuCharacter* GnuCharacter)
{
    if (GnuCharacter == nullptr) return;

    // Bind the delegate function to CrossHair's SetAimRate function
    GnuCharacter->func_Player_Aimrate.BindUObject(this, &UCrossHair::SetAimRate);
}
