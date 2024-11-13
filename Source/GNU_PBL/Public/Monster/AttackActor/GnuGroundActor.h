// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AttackActor/GnuAttackCollisionActor.h"
#include "GnuGroundActor.generated.h"


class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class GNU_PBL_API AGnuGroundActor : public AGnuAttackCollisionActor
{
	GENERATED_BODY()
	
public:
    AGnuGroundActor();

    UFUNCTION()
    void DestroyGround(); // 파이어브레스 삭제 함수

private:
    FTimerHandle DestructionTimerHandle;    // 타이머 핸들


protected:
    virtual void BeginPlay() override;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
    UNiagaraComponent* NiagaraComponent;


    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
