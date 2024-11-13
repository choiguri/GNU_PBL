// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GnuMonsterAIController.generated.h"


class UAIPerceptionComponent;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGnuMonsterAIController();

	virtual void Tick(float DeltaSecond) override;

	// BehaviorTree 종료 함수 (몬스턱 죽었을 때 호출)
	UFUNCTION()
	void StopBehaviorTree();

	//~ Begin IGenericTeamAgentInterface Interface.
	/*virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;*/
	//~ End IGenericTeamAgentInterface Interface

protected:
	virtual void BeginPlay() override;


	// 인식 시키는 컴포넌트 추가
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* AIBehavior;


	// 인식 관련 컴포넌트
	void SetUpPerceptionComponent();

	UFUNCTION()
	void UpdateTarget();

	void SetNewTarget(ACharacter* NewTarget);

	bool IsCloser(ACharacter* NewTarget, ACharacter* CurrentTarget);

	// Target 지정하는 함수 지정 (Array 형식으로 Actor들 인식시키기)
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);


private:
	
	FTimerHandle TargetUpdateTimerHandle;

	float TargetUpdateInterval;
};
