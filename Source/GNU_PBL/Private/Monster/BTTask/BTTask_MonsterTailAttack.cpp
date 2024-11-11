// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_MonsterTailAttack.h"
#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"
#include "AIController.h"

UBTTask_MonsterTailAttack::UBTTask_MonsterTailAttack()
{
	NodeName = TEXT("TailAttack");
	// TickTask 활성화
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MonsterTailAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		AnimInstance->PlayTailAttackMontage();
		return EBTNodeResult::InProgress;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}

void UBTTask_MonsterTailAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AGnuMonster* Monster = Cast<AGnuMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (Monster == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(Monster->GetMesh()->GetAnimInstance());
	if (AnimInstance && AnimInstance->bIsMontageEnded) // 몽타주가 끝났는지 확인
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 완료되면 Succeeded 반환
	}
}
