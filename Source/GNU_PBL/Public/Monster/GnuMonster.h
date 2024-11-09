// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GnuMonsterAnimInstance.h"
#include "Monster/Widget/GnuMonsterHealthBase.h"
#include "GnuMonster.generated.h"


class UGnuMonsterHealthBase;
class UBoxComponent;
class UAnimMontage;
class AGnuMyCharacter;


UCLASS()
class GNU_PBL_API AGnuMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGnuMonster();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// TakeDamage 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// HP 상황에 따라 상태 함수들
	void Die();
	void EnterPhaseTwo();

	// 몬스터 공격 함수
	void SpawnFireball();  // 파이어볼 소환 함수
	void SpawnFiretornado(); // 파이어토네이도 소환 함수
	void SpawnFirebreath(); // 파이어브레스 소환 함수

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float KnockbackStrength; // 넉백 힘

	// 몬스터 애님인스턴스 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UGnuMonsterAnimInstance> MonsterAnimInstanceClass;

	UFUNCTION()
	void InitializeAnimInstance();

	UFUNCTION()
	void InitializeCollisionComponent(UBoxComponent*& CollisionComponent, const FName& ComponentName);


	// 공격 부위별 콜리전 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	UBoxComponent* ClawCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	UBoxComponent* TailCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FName ClawCollisionBoxAttachBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FName TailCollisionBoxAttachBoneName;

#if WITH_EDITOR
	//~ Begin UObject Interface.
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
#endif


private:
	// 페이즈 2 여부 확인
	bool bIsPhaseTwo = false;

	// 충돌 처리 함수
	UFUNCTION()	// 몬스터가 맞았는지
	void OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()	// 몬스터 발톱 공격에 맞았는지
	void OnClawOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()	// 몬스터 꼬리 공격에 맞았는지
	void OnTailOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 근접 공격 맞았으면 플레이어가 뒤로 밀려나도록 하기
	void KnockbackPlayer(AGnuMyCharacter* PlayerCharacter);

public:
	// 블루프린트 위젯 넣는 곳
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGnuMonsterHealthBase> MonsterHealthWidgetClass;

	UGnuMonsterHealthBase* MonsterHealthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* MonsterWidget;

	// HP 관련 구현부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;


	// 원거리 공격 Actor 소환 class 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class AGnuFireballActor> FireballClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class AGnuFiretornadoActor> FiretornadoClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class AGnuFirebreathActor> FirebreathClass;
	UPROPERTY()
	AGnuFirebreathActor* FirebreathActor;

	// 근접 공격 콜리전 활성, 비활성
	void ActivateClawCollision();
	void DeactivateClawCollision();

	void ActivateTailCollision();
	void DeactivateTailCollision();

};