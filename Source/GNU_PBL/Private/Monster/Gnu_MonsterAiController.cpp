// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Gnu_MonsterAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"


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
		APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		// BlackboardComponent.h에서 가져온 함수들
		//TEXT() 이름과 BB 선언 Key와 이름이 맞아야함
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
}

void AGnu_MonsterAiController::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (LineOfSightTo(PlayerPawn))
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());

		/*
		// 타겟팅 함수는 (Actor)SetFocalPoint, SetFocus, ClearFocus가 있음
		SetFocus(PlayerPawn);
		// 움직임 함수는 (AiController)MoveTo, MoveToLocation MoveToActor가 있음
		// 대상, 가까이 갈 거리
		MoveToActor(PlayerPawn, AcceptanceRadius);
		*/
	}
	else 
	{
		// PlayerLocation 키 값을 초기화 시키기
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLoaction"));

		/*
		// 두 함수 모두 AiController에 있음
		// 우선순위를 바꿔주기 위해 GamePlay를 가져옴
		ClearFocus(EAIFocusPriority::Gameplay);
		StopMovement();
		*/
	}

}