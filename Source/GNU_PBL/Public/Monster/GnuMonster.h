// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GnuMonsterAnimInstance.h"
#include "GnuMonster.generated.h"


UCLASS()
class GNU_PBL_API AGnuMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGnuMonster();

	void SpawnFireball();  // 파이어볼 소환 함수

	void SpawnFiretornado(); // 파이어토네이도 소환 함수

	void SpawnFirebreath(); // 파이어브레스 소환 함수

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 몬스터 애님인스턴스 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UGnuMonsterAnimInstance> MonsterAnimInstanceClass;

	UFUNCTION()
	void InitializeAnimInstance();

private:


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* FlyingAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* FirebreathAttackMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class AGnuFireballActor> FireballClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class AGnuFiretornadoActor> FiretornadoClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class AGnuFirebreathActor> FirebreathClass;

	UPROPERTY()
	AGnuFirebreathActor* FirebreathActor;


	UFUNCTION()
	void PlayDefaultAttackMontage();

	UFUNCTION()
	void PlayFlyingAttackMontage();

	UFUNCTION()
	void PlayFirebreathAttackMontage();
};