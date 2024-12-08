// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_CustomMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTTask_CustomMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
public:
	// Constructor
	UBTTask_CustomMoveTo();

protected:
	// Execute the task
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// 현재 목표 위치를 저장하여 변경 여부를 확인
	FVector CurrentTargetLocation;

	// 랜덤 반경
	float RandomAcceptanceRadius;
public:
	// 최소 반경과 최대 반경을 설정할 변수
	UPROPERTY(EditAnywhere, Category = "MoveTo Settings")
	float MinRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "MoveTo Settings")
	float MaxRadius = 2000.0f;

	// 이동 목표를 블랙보드 키로 설정
	UPROPERTY(EditAnywhere, Category = "MoveTo Settings")
	FBlackboardKeySelector TargetLocationKey;
};
