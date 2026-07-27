// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_CustomMoveTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"  // 랜덤 숫자 생성을 위한 헤더
#include "Math/UnrealMathUtility.h"  // FVector::RandCone을 위한 헤더
#include "Navigation/PathFollowingComponent.h"
#include "NavFilters/NavigationQueryFilter.h"


UBTTask_CustomMoveTo::UBTTask_CustomMoveTo()
{
    NodeName = "Custom MoveTo";
    bNotifyTick = true; // TickTask 활성화
}

EBTNodeResult::Type UBTTask_CustomMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 블랙보드와 AIController 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!BlackboardComp || !AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("Blackboard or AIController is invalid!"));
        return EBTNodeResult::Failed;
    }

    // 현재 TargetLocation 값 저장
    CurrentTargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);

    // 허용 가능 반경을 최소~최대 값 사이에서 랜덤으로 설정
    RandomAcceptanceRadius = FMath::RandRange(MinRadius, MaxRadius);

    ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());
    if (ControlledCharacter)
    {
        float DistanceToTarget = FVector::Dist(ControlledCharacter->GetActorLocation(), CurrentTargetLocation);
        if (DistanceToTarget <= RandomAcceptanceRadius)
        {
            UE_LOG(LogTemp, Warning, TEXT("Already within acceptance radius."));
            return EBTNodeResult::Succeeded; // 목표 반경 안에 있으면 즉시 성공
        }
    }

    // 첫 MoveToLocation 호출
    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
        CurrentTargetLocation,
        RandomAcceptanceRadius,
        true // 직선 경로 여부
    );

    if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
    {
        return EBTNodeResult::InProgress; // 이동 시작
    }

    UE_LOG(LogTemp, Warning, TEXT("Initial MoveToLocation failed."));
    return EBTNodeResult::Failed;
}

void UBTTask_CustomMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BlackboardComp) return;

    UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent();
    if (!PathFollowingComp) return;

    // 현재 위치와 목표 위치 비교
    ACharacter* ControlledCharacter = Cast<ACharacter>(AIController->GetPawn());
    if (ControlledCharacter)
    {
        float DistanceToTarget = FVector::Dist(ControlledCharacter->GetActorLocation(), CurrentTargetLocation);
        if (DistanceToTarget <= RandomAcceptanceRadius)
        {
            UE_LOG(LogTemp, Warning, TEXT("Reached target within acceptance radius during tick."));
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 목표 반경 안에 도달하면 태스크 성공
            return;
        }
    }

    // 새로운 TargetLocation 확인
    FVector NewTargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);
    if (!NewTargetLocation.Equals(CurrentTargetLocation, 50.0f)) // 새로운 위치가 다를 경우
    {
        CurrentTargetLocation = NewTargetLocation;

        // 이동 명령 업데이트
        AIController->MoveToLocation(
            CurrentTargetLocation,
            RandomAcceptanceRadius,
            true
        );

        UE_LOG(LogTemp, Warning, TEXT("Updated MoveToLocation with new target."));
    }

    // 현재 이동 상태 확인
    const EPathFollowingStatus::Type Status = PathFollowingComp->GetStatus();
    if (Status == EPathFollowingStatus::Idle)
    {
        // 목표에 도달했거나 실패 시 태스크 종료
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}