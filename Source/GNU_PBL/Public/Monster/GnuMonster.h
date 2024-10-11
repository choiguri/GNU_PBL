// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GnuFireballActor.h"
#include "Animation/AnimNotifies/AnimNotify.h"
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

	void SpawnFireball();  // 파이어볼 발사 함수

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class AGnuFireballActor> FireballClass;

	UFUNCTION()
	void PlayAttackMontage();

};