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
    void SetAimRate(float Aimrate);

    void BindUserAimRate(class AGnuCharacter* GnuCharacter);

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* Cross_L; // 왼쪽 조준점

    UPROPERTY(meta = (BindWidget))
    UImage* Cross_R; // 오른쪽 조준점

    UPROPERTY(meta = (BindWidget))
    UImage* Cross_T; // 위쪽 조준점

    UPROPERTY(meta = (BindWidget))
    UImage* Cross_B; // 아래쪽 조준점

private:
    float fTarget_Aimrate;
    FDele_Player_Aimrate func_Player_Aimrate; 
};
