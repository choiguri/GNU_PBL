// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Image.h>
#include "CrossHair.generated.h"



DECLARE_DELEGATE_OneParam(FDele_Player_Aimrate, float);

UCLASS()
class GNU_PBL_API UCrossHair : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void NativeConstruct();
    
    void UpdateCrossHair(float aimrate);

    void SetAimRate(float Aimrate);

    void BindUserAimRate(class AGnuMyCharacter* GnuMyCharacter);

    void BindUserAimRate(class AGnuCharacter* GnuCharacter);

protected:
    UWidget* UICross_L; // Left
    UWidget* UICross_R; // Right
    UWidget* UICross_T; // Top
    UWidget* UICross_B; // Bottom 
    UWidget* UICross_C; // Center 

private:
    float fTarget_Aimrate = 15.f;
    FDele_Player_Aimrate func_Player_Aimrate; 
};
