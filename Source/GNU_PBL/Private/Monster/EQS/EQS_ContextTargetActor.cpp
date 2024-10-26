// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/EQS/EQS_ContextTargetActor.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

void UEQS_ContextTargetActor::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    // Querier를 가져옵니다 (EQS 쿼리를 요청한 객체)
    UObject* QueryOwner = QueryInstance.Owner.Get();

    // Querier가 AIController로부터 블랙보드를 사용할 수 있는지 확인
    AAIController* AIController = Cast<AAIController>(QueryOwner);
    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            // 블랙보드에서 TargetActor(또는 원하는 오브젝트)를 가져오기
            UObject* TargetObject = BlackboardComp->GetValue<UBlackboardKeyType_Object>(BlackboardComp->GetKeyID("TargetActor"));

            if (TargetObject)
            {
                // TargetActor의 위치를 EQS 쿼리에 컨텍스트로 추가
                AActor* TargetActor = Cast<AActor>(TargetObject);
                if (TargetActor)
                {
                    // 결과로 추가하여 EQS 쿼리에서 사용
                    UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
                }
            }
        }
    }
}
