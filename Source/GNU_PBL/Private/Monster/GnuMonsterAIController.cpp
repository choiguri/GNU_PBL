#include "Monster/GnuMonsterAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Characters/GnuMyCharacter.h"
#include "Monster/GnuMonster.h"
#include "TimerManager.h"

AGnuMonsterAIController::AGnuMonsterAIController()
{
    SetUpPerceptionComponent();
}

void AGnuMonsterAIController::BeginPlay()
{
    Super::BeginPlay();


    // BehaviorTree 실행
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
    ACharacter* CurrentTarget = Cast<ACharacter>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));

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
            GetBlackboardComponent()->SetValueAsFloat(TEXT("DistToTarget"), Distance);
        }
    }
    else if (bCanRetry && CurrentTarget == nullptr)
    {
        // 타겟이 없으면 재시도
        UpdateTarget();
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("No Target Detected. Retrying..."));

        // 재시도 제한 시작
        StartRetryCooldown();
    }
}


void AGnuMonsterAIController::StopBehaviorTree()
{
    if (BrainComponent)
    {
        BrainComponent->StopLogic(TEXT("Monster Died"));
    }
}

// 인식 설정 함수
void AGnuMonsterAIController::SetUpPerceptionComponent()
{
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

        SightConfig->SightRadius = 4000.f; // 감지 반경
        SightConfig->LoseSightRadius = 5000.0f; // 감지 상실 반경
        SightConfig->PeripheralVisionAngleDegrees = 360.0f; // 시야 각도
        SightConfig->SetMaxAge(60.0f); // 감지 정보 최대 시간
        SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true; // 중립 감지
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // 아군 감지

        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

        // 감지 이벤트 연결
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnTargetDetected);
    }
}


// 초기 타겟 지정
void AGnuMonsterAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (!GetBlackboardComponent() || !Actor || !Actor->IsValidLowLevel())
    {
        return;
    }

    // 타겟 탐지 성공 시
    if (Stimulus.WasSuccessfullySensed())
    {
        // SensedCharacter가 특정 클래스인지 확인
        AGnuMyCharacter* SensedCharacter = Cast<AGnuMyCharacter>(Actor);
        if (SensedCharacter && SensedCharacter->IsA<AGnuMyCharacter>())
        {
            // 기존 타겟 가져오기
            AGnuMyCharacter* CurrentTarget = Cast<AGnuMyCharacter>(GetBlackboardComponent()->GetValueAsObject(FName("TargetActor")));

            // 기존 타겟이 없거나, 가장 가까운 플레이어를 찾기
            if (!CurrentTarget || IsCloser(SensedCharacter, CurrentTarget))
            {
                SetNewTarget(SensedCharacter);
            }
        }

        // 콜리전 활성화 시키기
        APawn* ControlledPawn = GetPawn(); // AI가 컨트롤하는 Pawn
        AGnuMonster* GnuMonster = Cast<AGnuMonster>(ControlledPawn);
        if (GnuMonster)
        {
            GnuMonster->ActivateSkeletalMesh();
            GnuMonster->ActivateCapsuleComp();
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, TEXT("Failed ActivateCollision!!"));
        }
        
        // 인트로 몽타주 설정
        if (ControlledPawn)
        {
            // 몬스터 캐스트
            AGnuMonster* ControlledMonster = Cast<AGnuMonster>(ControlledPawn);
            if (ControlledMonster)
            {
                // AnimInstance 가져오기
                UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(ControlledMonster->GetMesh()->GetAnimInstance());
                if (AnimInstance && AnimInstance->bIsPlayIntro == false)
                {
                    // Intro Montage 실행
                    AnimInstance->PlayIntroMontage();
                }
            }
        }
    }
    else
    {
        // 타겟 상실 시 블랙보드에서 TargetActor와 PlayerLocation 초기화
        /*GetBlackboardComponent()->ClearValue(FName("TargetActor"));
        GetBlackboardComponent()->ClearValue(FName("PlayerLocation"));
        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, TEXT("Perception Target loss"));*/

        APawn* ControlledPawn = GetPawn(); // AI가 컨트롤하는 Pawn
        AGnuMonster* GnuMonster = Cast<AGnuMonster>(ControlledPawn);
        if (GnuMonster)
        {
            GnuMonster->DeactivateCapsuleComp();
            GnuMonster->DeactivateSkeletalMesh();
        }
        
        UpdateTarget();
    }
}

void AGnuMonsterAIController::StartRetryCooldown()
{
    bCanRetry = false; // 재시도 불가 상태로 변경

    // 일정 시간이 지난 후 재시도 가능 상태로 변경
    GetWorld()->GetTimerManager().SetTimer(
        RetryCooldownTimerHandle,
        [this]() { bCanRetry = true; },
        0.5f, // 쿨다운 시간 (초)
        false // 반복하지 않음
    );
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