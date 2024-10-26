// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_MonsterFirebreathAttack.h"
#include "Monster/GnuMonster.h"
#include "AIController.h"

UBTTask_MonsterFirebreathAttack::UBTTask_MonsterFirebreathAttack()
{
	NodeName = TEXT("FirebreathAttack");
}

EBTNodeResult::Type UBTTask_MonsterFirebreathAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	Monster->PlayFirebreathAttackMontage();


	return EBTNodeResult::Succeeded;
}
