// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Once.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API UBTDecorator_Once : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_Once();

protected:
    // Mutable : const로 선언된 메서드에서 해당 멤버 변수를 수정할 수 있도록 허용
    UPROPERTY()
    mutable bool bExecuted;

    // 블랙보드 키 배열 (부울 타입)
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    TArray<FBlackboardKeySelector> BoolKeys; // 여러 부울 블랙보드 키를 처리할 수 있도록 배열로 설정


    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
};
