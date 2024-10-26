// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/GnuEnemyBaseCharacter.h"
#include "GnuEnemyCharacter.generated.h"

// WarriorEnemyCharacter 파일

class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuEnemyCharacter : public AGnuEnemyBaseCharacter
{
	GENERATED_BODY()
	
public:
	AGnuEnemyCharacter();

protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

private:
	// 기본 캐릭터 함수에서 데이터를 사용할때 동기적으로 enemy와 설정을 하면
	// 수 많은 인스턴스 정보를 만들어내야해서 게임 실행시 데이터 처리 시간이 오래걸린다.
	// 비동기적 데이터 로드를 시키기 위해서 만든 함수
	void InitEnemyStartUpData();

public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};
