// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_MontsterExampleAnim.h"
#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"
#include "AIController.h"


UBTTask_MontsterExampleAnim::UBTTask_MontsterExampleAnim()
{
	NodeName = TEXT("Example Anim");
}


EBTNodeResult::Type UBTTask_MontsterExampleAnim::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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


	// 몬스터의 애니메이션 인스턴스를 가져오기
	UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(Monster->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayExampleMontage();
	}
	else
	{
		return EBTNodeResult::Failed;
	}


	return EBTNodeResult::Succeeded;
}

