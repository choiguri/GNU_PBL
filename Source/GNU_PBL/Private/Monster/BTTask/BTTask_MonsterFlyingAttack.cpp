// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_MonsterFlyingAttack.h"
#include "Monster/GnuMonster.h"
#include "AIController.h"

UBTTask_MonsterFlyingAttack::UBTTask_MonsterFlyingAttack()
{
	NodeName = TEXT("Flying Attack");
}

EBTNodeResult::Type UBTTask_MonsterFlyingAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AGnuMonster* Monster = Cast<AGnuMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (Monster == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Monster->PlayFlyingAttackMontage();


	return EBTNodeResult::Succeeded;
}