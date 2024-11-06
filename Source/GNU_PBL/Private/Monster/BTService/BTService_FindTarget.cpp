// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTService/BTService_FindTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Characters/GnuCharacter.h"                // GnuCharacter 헤더 포함

UBTService_FindTarget::UBTService_FindTarget()
{
	NodeName = "Find Target Object";
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // AIController 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    // AI의 Pawn과 PerceptionComponent 가져오기
    APawn* AIPawn = AIController->GetPawn();
    UAIPerceptionComponent* PerceptionComp = AIPawn->FindComponentByClass<UAIPerceptionComponent>();
    if (!PerceptionComp) return;

    // 탐지된 모든 Actors 배열 가져오기
    TArray<AActor*> PerceivedActors;
    PerceptionComp->GetCurrentlyPerceivedActors(nullptr, PerceivedActors);

    // GnuCharacter 타입 액터 탐색
    AActor* TargetActor = nullptr;
    for (AActor* Actor : PerceivedActors)
    {
        AGnuCharacter* GnuCharacter = Cast<AGnuCharacter>(Actor);
        if (GnuCharacter)
        {
            TargetActor = GnuCharacter;
            break;
        }
    }

    // Blackboard에 Target Actor 설정
    if (TargetActor)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), TargetActor);
    }
    else
    {
        // 시야 내에 GnuCharacter가 없을 때 Blackboard에서 Target 제거
        OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
    }
}
