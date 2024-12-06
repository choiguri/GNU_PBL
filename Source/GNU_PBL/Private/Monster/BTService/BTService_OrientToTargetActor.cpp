// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTService/BTService_OrientToTargetActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/GnuMonsterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

// 초기 설정
UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{
	NodeName = TEXT("Native Orient Rotation To Target Actor");

	RotationInterpSpeed = 2.f;
	Interval = 0.f;
	RandomDeviation = 0.f;

	// 상위 클래스에서 재정의해야 제대로 사용가능하다 => BTNode.h로 ㄱㄱ
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

// BTService 묘사
FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();

	return FString::Printf(TEXT("Orient roation to %s Key %s"), *KeyDescription, *GetStaticServiceDescription());
}

// 항상 실행 하는 곳
void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 컨트롤 + T 로 해당 포함해야되는 헤더파일 찾으러 갈 수 있음
	// cpp 라인의 first 헤더파일을 복붙한다
	// ex) BlackboardComponent()를 드래그 -> 컨트롤+T -> cpp 파일 클릭 -> 맨 위 헤더파일 복사 붙여넣기
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);

	if (ActorObject == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActorObject is null! Check if the Blackboard key is correct."));
	}

	AActor* TargetActor = Cast<AActor>(ActorObject);

	if (TargetActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is null! Make sure the Blackboard key is set to a valid target."));
	}

	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (OwningPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwningPawn is null! AIController may not be set correctly."));
	}

	// 둘다 유효할 때만 아래 로직 실행
	if (OwningPawn && TargetActor)
	{
		// AI가 타겟을 바라보는 방향 계산
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
		const FRotator TargetRot = FMath::RInterpTo(OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);

		OwningPawn->SetActorRotation(TargetRot);
	}
}
