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


	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;

	if (PlayerPawn)
	{
		if (AIPawn)
		{
			// Calculate the distance between the controlled pawn and the player
			float DistToPlayer = FVector::Dist(AIPawn->GetActorLocation(), PlayerPawn->GetActorLocation());

			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsFloat(TEXT("DistToTarget"), DistToPlayer);
			}
		}
	}
}
