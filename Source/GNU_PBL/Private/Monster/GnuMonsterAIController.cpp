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
#include "Net/UnrealNetwork.h"

AGnuMonsterAIController::AGnuMonsterAIController()
{
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    if (SightConfig)
    {
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

    bReplicates = true;
}

void AGnuMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    if (AIBehavior)
    {
        RunBehaviorTree(AIBehavior);
        if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
        {
            BlackboardComp->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
        }
    }

    TargetUpdateInterval = 15.f;    // 타겟 변경 주기 시간 설정
    GetWorld()->GetTimerManager().SetTimer(TargetUpdateTimerHandle, this, &ThisClass::UpdateTarget, TargetUpdateInterval, true);    // 타이머 설정
}

void AGnuMonsterAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (HasAuthority())
    {
        UpdateTargetDistance();     // 타겟과의 거리 계산
        UpdateMonsterHealth();      // 몬스터 현재 hp 블랙보드 키값 업데이트
    }

    ACharacter* CurrentTarget = Cast<ACharacter>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
    if (CurrentTarget == nullptr && bCanRetry)
    {
        UpdateTarget();

        GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("No Target Detected. Retrying..."));
        StartRetryCooldown();
    }
}

// 초기 타겟 감지 설정
void AGnuMonsterAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (!Stimulus.WasSuccessfullySensed() || !Actor || !Actor->IsA<ACharacter>() || !HasAuthority())
    {
        return;
    }

    ACharacter* DetectedCharacter = Cast<ACharacter>(Actor);
    if (DetectedCharacter && DetectedCharacter->IsA(AGnuMyCharacter::StaticClass()))
    {
        SetNewTarget(DetectedCharacter);
    }
}

// 꾸준히 감지할 타겟 업데이트 함수
void AGnuMonsterAIController::UpdateTarget()
{
    if (!HasAuthority() || !AIPerceptionComponent)
    {
        return;
    }

    TArray<AActor*> DetectedActors;     // 감지된 액터 보관할 배열
    AIPerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), DetectedActors); // 인식 AI로 감지된 모든 액터 가져오기

    ACharacter* NewTarget = nullptr;    // 가장 가까운 플레이어 저장할 변수
    float ClosestDistance = FLT_MAX;        // 가장 가까운 액터까지의 거리 저장할 변수

    for (AActor* Actor : DetectedActors)    // 감지된 액터 하나씩 순회
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(Actor);  // PlayerCharacter가 GnuMyCharacter인지 확인
        if (PlayerCharacter && PlayerCharacter->IsA(AGnuMyCharacter::StaticClass()))    // GnuMyCharacter로 대체
        {
            float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), GetPawn()->GetActorLocation());
            if (Distance < ClosestDistance)     // 더 가까운 플레이어 찾기
            {
                NewTarget = PlayerCharacter;
                ClosestDistance = Distance;
            }
        }
    }

    
    if (NewTarget)
    {
        SetNewTarget(NewTarget);    // 새 타겟 설정
        ActivateMonster();          // 몬스터 콜리전 활성화
    }
    else
    {
        ClearTarget(); // 인식한 타겟이 없으면 TargacActor 클리어
    }
}

// BehaviorTree 종료 함수
void AGnuMonsterAIController::StopBehaviorTree()
{
    if (BrainComponent)
    {
        BrainComponent->StopLogic(TEXT("Monster Died"));
    }
}

// TargetActor와 거리 계산
void AGnuMonsterAIController::UpdateTargetDistance()
{
    ACharacter* CurrentTarget = Cast<ACharacter>(GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
    if (CurrentTarget)
    {
        float Distance = FVector::Dist(CurrentTarget->GetActorLocation(), GetPawn()->GetActorLocation());
        GetBlackboardComponent()->SetValueAsFloat(TEXT("DistToTarget"), Distance);
    }
}

void AGnuMonsterAIController::UpdateMonsterHealth()
{
    AGnuMonster* GnuMonster = Cast<AGnuMonster>(GetPawn());
    if (GnuMonster)
    {
        GetBlackboardComponent()->SetValueAsFloat(TEXT("CurrentHealth"), GnuMonster->CurrentHealth);
    }
}

// 블랙보드 TargetActor 업데이트
void AGnuMonsterAIController::SetNewTarget(ACharacter* NewTarget)
{
    // 타겟 설정
    if (HasAuthority())
    {
        TargetActor = NewTarget;
        GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), NewTarget);
    }

    // 타겟을 처음 인식했을 때만 Intro Montage를 실행
    AGnuMonster* GnuMonster = Cast<AGnuMonster>(GetPawn());
    if (GnuMonster)
    {
        UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(GnuMonster->GetMesh()->GetAnimInstance());
        if (AnimInstance && AnimInstance->bIsPlayIntro)  // 아직 Intro를 실행하지 않았다면
        {
            // Intro Montage 실행
            AnimInstance->PlayIntroMontage();
            AnimInstance->bIsPlayIntro = false;  // 인트로 애니메이션을 실행했으므로 false로 설정
        }
    }
}

// 인식한 타겟이 사라지면 실행할 함수
void AGnuMonsterAIController::ClearTarget()
{
    // 타겟 삭제
    TargetActor = nullptr;
    GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));

    // 몬스터 콜리전 비활성화
    AGnuMonster* GnuMonster = Cast<AGnuMonster>(GetPawn());
    if (GnuMonster)
    {
        GnuMonster->DeactivateCapsuleComp();
        GnuMonster->DeactivateSkeletalMesh();
    }
}

void AGnuMonsterAIController::StartRetryCooldown()
{
    bCanRetry = false; // 재시도 불가 상태로 변경

    // 일정 시간이 지난 후 재시도 가능 상태로 변경
    GetWorld()->GetTimerManager().SetTimer(
        RetryCooldownTimerHandle,
        [this]() { bCanRetry = true; },
        1.0f, // 쿨다운 시간 (초)
        false // 반복하지 않음
    );
}

// 몬스터 콜리전 활성화 함수
void AGnuMonsterAIController::ActivateMonster()
{
    AGnuMonster* Monster = Cast<AGnuMonster>(GetPawn());
    if (Monster)
    {
        Monster->ActivateSkeletalMesh();
        Monster->ActivateCapsuleComp();
    }
}

void AGnuMonsterAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // TargetActor 리플리케이션
    DOREPLIFETIME(AGnuMonsterAIController, TargetActor);
}

void AGnuMonsterAIController::OnRep_TargetActor()
{
    GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), TargetActor);
}
