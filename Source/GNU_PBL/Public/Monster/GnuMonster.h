// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GnuMonster.generated.h"

class UCapsuleComponent;
class USphereComponent;

UCLASS()
class GNU_PBL_API AGnuMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGnuMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere)
	USphereComponent* DetectRange;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Overlap Functions
	UFUNCTION()
	void OnPlayerEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage;

	UFUNCTION()
	void PlayAttackMontage();

};