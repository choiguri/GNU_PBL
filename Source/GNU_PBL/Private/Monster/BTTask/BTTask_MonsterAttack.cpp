// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_MonsterAttack.h"
#include "Monster/GnuMonster.h"
#include "AIController.h"


UBTTask_MonsterAttack::UBTTask_MonsterAttack()
{
	NodeName = TEXT("DefaultAttack");
}

EBTNodeResult::Type UBTTask_MonsterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	Monster->PlayDefaultAttackMontage();

	
	return EBTNodeResult::Succeeded;
}
