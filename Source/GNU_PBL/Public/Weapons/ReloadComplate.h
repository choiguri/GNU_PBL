// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ReloadComplate.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UReloadComplate : public UAnimNotify
{
	GENERATED_BODY()

public:
	// Notify 호출 시 실행될 함수
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
