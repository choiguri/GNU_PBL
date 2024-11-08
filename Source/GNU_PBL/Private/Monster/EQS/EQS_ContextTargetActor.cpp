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
    // Querier 객체와 Actor를 가져옵니다.
    UObject* QuerierObject = QueryInstance.Owner.Get();
    AActor* QuerierActor = Cast<AActor>(QuerierObject);

    if (!QuerierActor)
    {
        return;
    }

    // Querier의 AIController와 BlackboardComponent를 확인합니다.
    AAIController* AIController = Cast<AAIController>(QuerierActor->GetInstigatorController());
    if (AIController && AIController->GetBlackboardComponent())
    {
        // Blackboard에서 TargetActor를 가져옵니다.
        UObject* TargetObject = AIController->GetBlackboardComponent()->GetValueAsObject(FName("TargetActor"));
        AActor* TargetActor = Cast<AActor>(TargetObject);

        if (TargetActor)
        {
            // ContextData에 TargetActor 추가
            UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
        }
    }
}