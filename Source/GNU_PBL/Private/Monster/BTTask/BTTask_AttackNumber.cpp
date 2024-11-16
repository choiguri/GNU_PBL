// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_AttackNumber.h"
#include "Monster/GnuMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"  // 로그 출력용

UBTTask_AttackNumber::UBTTask_AttackNumber()
{
	NodeName = TEXT("AttackNumber");

}

EBTNodeResult::Type UBTTask_AttackNumber::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 0~100 사이의 랜덤 숫자 생성
    float RandomNumber = UKismetMathLibrary::RandomIntegerInRange(0, 100);

    // 블랙보드에서 AttackNumber 키 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp)
    {
        // AttackNumber 값을 블랙보드에 저장
        BlackboardComp->SetValueAsFloat(TEXT("AttackSelectNumber"), RandomNumber);
    }

    return EBTNodeResult::Succeeded;
}