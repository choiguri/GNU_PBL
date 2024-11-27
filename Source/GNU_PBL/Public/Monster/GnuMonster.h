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
	//virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// HP 상황에 따라 상태 함수들
	void Die(); // 사망 함수
	void DelayedDestroy(); // 타이머가 만료되면 호출될 함수
	void EnterPhaseTwo(); // 페이즈 2 (할지 안할지 모름)

	// 콜리전 관련 함수
	void ActivateSkeletalMesh();
	void ActivateCapsuleComp();
	void DeactivateSkeletalMesh(); // 스켈레탈 메시 비활성화
	void DeactivateCapsuleComp(); // 캡슐 컴포넌트 비활성화

	// 몬스터 공격 함수
	UFUNCTION()
	void SpawnFireball(); // 파이어볼 소환 함수
	UFUNCTION(Server, Reliable)
	void Server_SpawnFireball();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnFireball();

	void SpawnFiretornado(); // 파이어토네이도 소환 함수
	void SpawnFirebreath(); // 파이어브레스 소환 함수
	void SpawnGroundAttack(); // 그라운드 돌 공격 소환 함수
	void SpawnGroundSpikeAttack(); // 그라운드 5방향 스파이크 소환 함수

	void BodyAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float KnockbackStrength; // 넉백 힘
	FTimerHandle DestroyTimerHandle;

	// 몬스터 애님인스턴스 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UGnuMonsterAnimInstance> MonsterAnimInstanceClass;

	UFUNCTION()
	void InitializeAnimInstance();

	UFUNCTION()
	void InitializeCollisionComponent(UBoxComponent*& CollisionComponent, const FName& ComponentName);
	

	// 공격 부위별 콜리전 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Collision")
	UBoxComponent* ClawCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Collision")
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

	// 머터리얼 인스턴스 다이나믹으로 선언
	UMaterialInstanceDynamic* DynamicMaterialInst_1st;
	UMaterialInstanceDynamic* DynamicMaterialInst_2nd;

	UFUNCTION()	// 몬스터 발톱 공격에 맞았는지
	void OnClawOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()	// 몬스터 꼬리 공격에 맞았는지
	void OnTailOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 근접 공격 맞았으면 플레이어가 뒤로 밀려나도록 하기
	void KnockbackPlayer(AGnuMyCharacter* PlayerCharacter);

	// Tick 에서 타겟 변경 재시도 쿨다운 타이머
	FTimerHandle RetryCooldownTimerHandle;
	bool bCanRetry = true;

	// 재시도 쿨다운 설정 함수
	void StartRetryCooldown();

public:
	// 블루프린트 위젯 넣는 곳
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGnuMonsterHealthBase> MonsterHealthWidgetClass;

	UGnuMonsterHealthBase* MonsterHealthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* MonsterWidget;

	// HP 관련 구현부
	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 1000.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Health")
	float CurrentHealth = 1.f;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	// 멀티 관련
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// 원거리 공격 Actor 소환 class 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|ActorClass")
	TSubclassOf<class AGnuFireballActor> FireballClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|ActorClass")
	TSubclassOf<class AGnuFiretornadoActor> FiretornadoClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|ActorClass")
	TSubclassOf<class AGnuFirebreathActor> FirebreathClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|ActorClass")
	TSubclassOf<class AGnuGroundActor> GroundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|ActorClass")
	TSubclassOf<class AGnuGroundSpikeActor> GroundSpikeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|ActorClass")	// Ground Spike의 각 위치(5방향) 별로 발사시킬 콜리전
	TSubclassOf<class AGnuGroundSpikeCollisionActor> GroundSpikeCollisionClass;


	// Tick에서 위치 처리를 위해
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Actor")
	AGnuFirebreathActor* FirebreathActor;

	// 근접 공격 콜리전 활성, 비활성
	UFUNCTION()
	void ActivateClawCollision();
	UFUNCTION()
	void DeactivateClawCollision();

	UFUNCTION()
	void ActivateTailCollision();
	UFUNCTION()
	void DeactivateTailCollision();
};