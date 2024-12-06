// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GnuResetAngleNotify.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UGnuResetAngleNotify : public UAnimNotify
{
	GENERATED_BODY()
	

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
