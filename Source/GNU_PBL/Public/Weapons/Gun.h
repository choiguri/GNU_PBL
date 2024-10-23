// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/AmmoDisplay.h"
#include <Components/TimelineComponent.h>
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

	/*	// Recoil Function

	void InitializeRecoilTimeline();
	UFUNCTION()
	void OnCameraRecoilProgress(FVector CameraRecoil);

	UFUNCTION()
	void OnBulletRecoilProgress(FVector BulletRecoil);

	UFUNCTION()
	void OnRecoilTimelineFinish();
	*/

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Particle")
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere, Category = "Particle")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Spec")
	int Power = 50;

	UPROPERTY(EditAnywhere, Category = "Spec")
	float MaxRange = 1000;

	UPROPERTY(EditAnywhere, Category = "Spec")
	int MaxAmmo = 30;

	UPROPERTY(EditAnywhere, Category = "Spec")
	int RemainAmmo = MaxAmmo;
	UPROPERTY(EditAnywhere)
	float ReroadingDelay = 3.f;
	UPROPERTY(EditAnywhere, Category = "Spec")
	float Accuracy = 1.f;
	UPROPERTY(EditAnywhere, Category = "Spec")
	int RPM = 600;

	UPROPERTY(EditDefaultsOnly, Category = "Spec")
	float RecoilRecoveryTime;

	float CurrentRecoilRecoveryTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline", Meta = (AllowPrivateAccess = "true"))
	UCurveVector* CameraRecoilCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline", Meta = (AllowPrivateAccess = "true"))
	UCurveVector* BulletRecoilCurve;

	FTimeline RecoilTimeline;


	// AmmoDisplay 위젯의 클래스 변수
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UAmmoDisplay> AmmoDisplayClass;
		
	// AmmoDisplay 위젯 인스턴스
	UPROPERTY()
	UAmmoDisplay* AmmoDisplay;


	FTimerHandle FireTimerHandle;  // Ÿ�̸� �ڵ� ���� �߰�

	void Fire();  // �߻� ��� ���� �Լ�



};
