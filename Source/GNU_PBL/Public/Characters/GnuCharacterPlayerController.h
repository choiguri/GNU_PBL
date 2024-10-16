// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GnuCharacterPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGnuCharacterPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputMappingContext> MyCharacterContext;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> RotationAction;


	void Move(const struct FInputActionValue& InputActionValue);
	void Rotation(const struct FInputActionValue& InputActionValue);
};
