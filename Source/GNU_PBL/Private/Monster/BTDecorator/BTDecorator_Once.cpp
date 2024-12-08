// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTDecorator/BTDecorator_Once.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_Once::UBTDecorator_Once()
{
    NodeName = "Once Decorator";
    bNotifyActivation = true; // 노드 활성화 알림
}

bool UBTDecorator_Once::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    if (OwnerComp.GetBlackboardComponent())
    {
        // 부울 키값이 하나라도 false면 true로 변경하도록 함
        for (const FBlackboardKeySelector& BoolKey : BoolKeys)
        {
            if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(BoolKey.SelectedKeyName) == false)
            {
                return true; // 하나라도 false일 경우 true 반환 (조건 만족)
            }
        }
    }
    return false; // 모두 true일 경우 false 반환
}

void UBTDecorator_Once::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
    if (UBlackboardComponent* Blackboard = SearchData.OwnerComp.GetBlackboardComponent())
    {
        // 모든 부울 블랙보드 키 값의 true/false 상태를 토글
        for (const FBlackboardKeySelector& BoolKey : BoolKeys)
        {
            bool bCurrentValue = Blackboard->GetValueAsBool(BoolKey.SelectedKeyName);
            Blackboard->SetValueAsBool(BoolKey.SelectedKeyName, !bCurrentValue); // true면 false, false면 true로 토글
        }
    }

    Super::OnNodeActivation(SearchData);
}