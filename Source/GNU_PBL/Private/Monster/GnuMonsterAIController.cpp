#include "Monster/GnuMonsterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Characters/GnuMyCharacter.h"
#include "Monster/GnuMonster.h"
#include "TimerManager.h"

AGnuMonsterAIController::AGnuMonsterAIController()
{
    SetUpPerceptionComponent();

    // 팀 아이디 1로 지정
    SetGenericTeamId(FGenericTeamId(1));

    if (GetGenericTeamId() == FGenericTeamId(1))
    {
        UE_LOG(LogTemp, Warning, TEXT("Team ID successfully set to 1."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to set Team ID to 1. Current Team ID: %d"), GetGenericTeamId().GetId());
    }

}

void AGnuMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    if (AIBehavior != nullptr)
    {
        RunBehaviorTree(AIBehavior);

        if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
        {
            BlackboardComp->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
        }
    }

    // 타겟 변경 시간 설정
    TargetUpdateInterval = 15.f;

    // 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(TargetUpdateTimerHandle, this, &ThisClass::UpdateTarget, TargetUpdateInterval, true);
}


void AGnuMonsterAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 플레이어 캐릭터를 감지하여 타겟을 업데이트
    // 현재 타겟 업데이트
    ACharacter* CurrentTarget = Cast<ACharacter>(GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));

    if (CurrentTarget)
    {
        // 타겟의 위치를 계속 업데이트
        /*GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), CurrentTarget);*/
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), CurrentTarget->GetActorLocation());

        // GnuMonster의 위치 가져오기
        AGnuMonster* MonsterActor = Cast<AGnuMonster>(GetPawn());
        if (MonsterActor)
        {
            // 두 Actor 간의 거리 계산
            float Distance = FVector::Dist(MonsterActor->GetActorLocation(), CurrentTarget->GetActorLocation());

            // DistToTarget 블랙보드 키에 저장
            GetBlackboardComponent()->SetValueAsFloat(FName("DistToTarget"), Distance);
        }
    }
}

ETeamAttitude::Type AGnuMonsterAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    const APawn* PawnToCheck = Cast<const APawn>(&Other);

    const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(PawnToCheck->GetController());

    if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() != GetGenericTeamId())
    {
        return ETeamAttitude::Hostile;
    }
    return ETeamAttitude::Friendly;
}


// 인식 설정 함수
void AGnuMonsterAIController::SetUpPerceptionComponent()
{
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

        SightConfig->SightRadius = 3000.f; // 감지 반경
        SightConfig->LoseSightRadius = 4000.0f; // 감지 상실 반경
        SightConfig->PeripheralVisionAngleDegrees = 140.0f; // 시야 각도
        SightConfig->SetMaxAge(10.0f); // 감지 정보 최대 시간
        SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true; // 중립 감지
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // 아군 감지

        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

        // 감지 이벤트 연결
        /*AIPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnTargetDetected);*/
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AGnuMonsterAIController::OnTargetDetected);

    }
}


// 초기 타겟 지정
void AGnuMonsterAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (!GetBlackboardComponent() || !Actor)
    {
        return;
    }

    // 타겟 탐지 성공 시
    if (Stimulus.WasSuccessfullySensed())
    {
        // SensedCharacter가 특정 클래스인지 확인
        ACharacter* SensedCharacter = Cast<ACharacter>(Actor);
        if (SensedCharacter && SensedCharacter->IsA<AGnuMyCharacter>())
        {
            // 기존 타겟 가져오기
            ACharacter* CurrentTarget = Cast<ACharacter>(GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));

            // 기존 타겟이 없거나, 가장 가까운 플레이어를 찾기
            if (!CurrentTarget || IsCloser(SensedCharacter, CurrentTarget))
            {
                SetNewTarget(SensedCharacter);
            }
        }
    }
}


// 새 타겟 지정
void AGnuMonsterAIController::SetNewTarget(ACharacter* NewTarget)
{
    GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), NewTarget);
    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), NewTarget->GetActorLocation());

    UE_LOG(LogTemp, Warning, TEXT("New TargetActor set: %s"), *NewTarget->GetName());
}

// 가까운 캐릭터 찾기
bool AGnuMonsterAIController::IsCloser(ACharacter* NewTarget, ACharacter* CurrentTarget)
{
    float NewTargetDistance = FVector::Dist(NewTarget->GetActorLocation(), GetPawn()->GetActorLocation());
    float CurrentTargetDistance = FVector::Dist(CurrentTarget->GetActorLocation(), GetPawn()->GetActorLocation());
    return NewTargetDistance < CurrentTargetDistance;
}


// 타겟 업데이트 함수
void AGnuMonsterAIController::UpdateTarget()
{
    ACharacter* CurrentTarget = Cast<ACharacter>(GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));
    TArray<AActor*> DetectedActors;
    AIPerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), DetectedActors);

    ACharacter* NewTarget = nullptr;
    float ClosestDistance = FLT_MAX;

    for (AActor* Actor : DetectedActors)
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(Actor);
        // PlayerCharacter가 GnuMyCharacter인지 확인
        if (PlayerCharacter && PlayerCharacter->IsA(AGnuMyCharacter::StaticClass())) // GnuMyCharacter로 대체
        {
            float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), GetPawn()->GetActorLocation());

            // 현재 타겟이 없거나 가까운 플레이어를 찾기
            if (CurrentTarget == nullptr || Distance < ClosestDistance)
            {
                NewTarget = PlayerCharacter;
                ClosestDistance = Distance;
            }
        }
    }

    // 새 타겟 설정
    if (NewTarget)
    {
        SetNewTarget(NewTarget);
    }
}



//// 타겟 업데이트 함수
//void AGnuMonsterAIController::UpdateTarget()
//{
//    TArray<AActor*> DetectedActors;
//    AIPerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), DetectedActors);
//
//    ACharacter* ClosestPlayer = nullptr;
//    float ClosestDistance = FLT_MAX;
//
//    for (AActor* Actor : DetectedActors)
//    {
//        if (ACharacter* PlayerCharacter = Cast<ACharacter>(Actor))
//        {
//            float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), GetPawn()->GetActorLocation());
//            if (Distance < ClosestDistance)
//            {
//                ClosestDistance = Distance;
//                ClosestPlayer = PlayerCharacter;
//            }
//        }
//    }
//
//    if (ClosestPlayer)
//    {
//        GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), ClosestPlayer);
//        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), ClosestPlayer->GetActorLocation());
//    }
//    else
//    {
//        // 타겟이 없을 경우 초기화
//        GetBlackboardComponent()->ClearValue(FName("TargetActor"));
//        GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
//    }
//}

