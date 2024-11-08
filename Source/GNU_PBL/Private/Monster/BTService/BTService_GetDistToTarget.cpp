// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTService/BTService_GetDistToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_GetDistToTarget::UBTService_GetDistToTarget()
{
	NodeName = TEXT("DistToTarget");
}

void UBTService_GetDistToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // AI 컨트롤러를 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIController is null in TickNode"));
        return;
    }

    // 블랙보드 컴포넌트를 가져오기
    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("BlackboardComp is null in TickNode"));
        return;
    }

    // TargetActor를 가져오기
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetActor is null in TickNode"));
        return;
    }

    // AI 몬스터의 위치를 가져오기
    AActor* MonsterActor = AIController->GetPawn();
    if (!MonsterActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("MonsterActor is null in TickNode"));
        return;
    }

    // 두 Actor 간의 거리 계산
    float Distance = FVector::Dist(MonsterActor->GetActorLocation(), TargetActor->GetActorLocation());

    // DistToTarget 블랙보드 키에 저장
    BlackboardComp->SetValueAsFloat("DistToTarget", Distance);
    UE_LOG(LogTemp, Log, TEXT("Distance to target: %f"), Distance);
}
