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

	// BehaviorTree 종료 함수 (몬스터 죽었을 때 호출)
	UFUNCTION()
	void StopBehaviorTree();

	//// 몬스터를 참조하기 위한 변수
	//UPROPERTY()
	//AGnuMonster* GnuMonster;

	// Behaviortree 변수들 레플리케이트
	UPROPERTY(ReplicatedUsing = OnRep_TargetActor)
	AActor* TargetActor;

	UPROPERTY(ReplicatedUsing = OnRep_TargetLocation)
	FVector TargetLocation;

	UPROPERTY(ReplicatedUsing = OnRep_DistToTarget)
	float TargetDist;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_TargetActor();

	UFUNCTION()
	void OnRep_TargetLocation();

	UFUNCTION()
	void OnRep_DistToTarget();



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

	// 멀티
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
	// 타겟 변경 타이머
	FTimerHandle TargetUpdateTimerHandle;
	float TargetUpdateInterval;


	// Tick 에서 타겟 변경 재시도 쿨다운 타이머
	FTimerHandle RetryCooldownTimerHandle;
	bool bCanRetry = true;

	// 재시도 쿨다운 설정 함수
	void StartRetryCooldown();
};
