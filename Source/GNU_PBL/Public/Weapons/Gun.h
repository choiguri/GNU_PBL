// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/AmmoDisplay.h"
#include "Weapons/Bullet.h"
#include "Gun.generated.h"

UCLASS()
class GNU_PBL_API AGun : public AActor
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PullTrigger();
	virtual void ReleaseTrigger();
	virtual void Reload();
	float GetRecoilOffset();

	// 탄약 상태를 UI에 업데이트하는 함수
	void UpdateAmmoDisplay();

	void RemoveAmmoDisplay();
	

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditAnywhere)
	int Power = 50;

	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;

	UPROPERTY(EditAnywhere)
	int MaxAmmo = 30;

	UPROPERTY(EditAnywhere)
	int RemainAmmo = MaxAmmo;
	UPROPERTY(EditAnywhere)
	float ReroadingDelay = 3.f;
	UPROPERTY(EditAnywhere)
	float Accuracy = 1.f;
	UPROPERTY(EditAnywhere)
	int RPM = 600;

	// AmmoDisplay 위젯의 클래스 변수
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UAmmoDisplay> AmmoDisplayClass;
		
	// AmmoDisplay 위젯 인스턴스
	UPROPERTY()
	UAmmoDisplay* AmmoDisplay;


	FTimerHandle FireTimerHandle;  // Ÿ�̸� �ڵ� ���� �߰�

	void Fire();  // �߻� ��� ���� �Լ�



};
