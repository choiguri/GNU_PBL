// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BTTask/BTTask_MonsterAttack.h"
#include "Monster/GnuMonster.h"
#include "Monster/GnuMonsterAnimInstance.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"


UBTTask_MonsterAttack::UBTTask_MonsterAttack()
{
	NodeName = TEXT("FireballAttack");
	// TickTask 활성화
	bNotifyTick = true;
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

	// 몬스터의 애니메이션 인스턴스를 가져오기
	UGnuMonsterAnimInstance* AnimInstance = Cast<UGnuMonsterAnimInstance>(Monster->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayFireballAttackMontage(); // Fireball 공격 몽타주 실행
		// 몽타주가 끝날 때까지 기다리기 위해 Latent Task가 끝나기를 기다림
		return EBTNodeResult::InProgress;
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	
}

void UBTTask_MonsterAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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