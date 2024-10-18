// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Texture2D.h" 
#include "HUD_CrossHair.generated.h"


USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	// 크로스헤어의 각 텍스처
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	UTexture2D* CrossHairCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	UTexture2D* CrossHairLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	UTexture2D* CrossHairRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	UTexture2D* CrossHairTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	UTexture2D* CrossHairBottom;

	// 크로스헤어 퍼짐을 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	float CrossHairSpread;
};
/**
 * 
 */
UCLASS()
class GNU_PBL_API AHUD_CrossHair : public AHUD
{
	GENERATED_BODY()
	
public:
	AHUD_CrossHair();
	virtual void DrawHUD() override;

protected:
	// 크로스헤어 HUD 패키지 (크로스헤어 텍스처와 퍼짐 포함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	FHUDPackage HUDPackage;

	// 크로스헤어의 퍼짐 정도를 설정하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrossHair")
	FVector2D Spread;
};


