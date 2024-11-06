// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Gnu_MonsterAiController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


void AGnu_MonsterAiController::BeginPlay()
{
	// 상속 파일로 바로 가는 법 : 이름 -> F12
	// Super를 안쓰면 블루프린트에서 문제가 생길 수 있다
	Super::BeginPlay();


	if (AIBehavior != nullptr)
	{
		// BehaviorTree 실행 가능 함수
		RunBehaviorTree(AIBehavior);

		// pawn 캐릭터를 쳐다보도록 설정, Actor.h에 가면 있음
		//APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		// BlackboardComponent.h에서 가져온 함수들
		//TEXT() 이름과 BB 선언 Key와 이름이 맞아야함
		/*GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());*/
	}
}

// CrowFollowingComponent 를 써야 enemy들이 서로 안부딪히고 이동가능
AGnu_MonsterAiController::AGnu_MonsterAiController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	// AI 감지 구성
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;				// 적감지
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;			// 아군 감지
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;			// 중립 감지
	AISenseConfig_Sight->SightRadius = 5000.f;										// 측면 반경
	AISenseConfig_Sight->LoseSightRadius = 0.f;										// 0으로 설정하면 플레이어 절대 안놓침 
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;						// 주변 시야 각도

	// AI 인식 구성
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);
	// 탐지가 되면 해당 함수 호출되도록 만듦
}

void AGnu_MonsterAiController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		// 감지 성공 시 TargetActor 설정
		GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), Actor);
	}
	else
	{
		// 감지 상실 시 TargetActor 초기화
		GetBlackboardComponent()->ClearValue(FName("TargetActor"));
	}
}


void AGnu_MonsterAiController::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);

	/*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);*/

	// 강제로 Target Actor 설정
	//if (PlayerPawn)
	//{
	//	GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), PlayerPawn);
	//	/*GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Player Sensed"));*/
	//}


	//if (LineOfSightTo(PlayerPawn))
	//{
	//	/*GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	//	GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());*/
	//
	//	// 타겟팅 함수는 (Actor)SetFocalPoint, SetFocus, ClearFocus가 있음
	//	/*SetFocus(PlayerPawn);*/
	//	// 움직임 함수는 (AiController)MoveTo, MoveToLocation MoveToActor가 있음
	//	// 대상, 가까이 갈 거리
	//	/*MoveToActor(PlayerPawn, AcceptanceRadius);*/
	//}
	//else 
	//{
	//	// PlayerLocation 키 값을 초기화 시키기
	//	/*GetBlackboardComponent()->ClearValue(TEXT("PlayerLoaction"));*/
	//	
	//	// 두 함수 모두 AiController에 있음
	//	// 우선순위를 바꿔주기 위해 GamePlay를 가져옴
	//	// 포커스를 클리어해서 바라보는 행동 중지
	//	/*ClearFocus(EAIFocusPriority::Gameplay);
	//	StopMovement();*/
	//	
	//}

}