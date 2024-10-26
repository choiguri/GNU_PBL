// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonster.h"
#include "Monster/AttackActor/GnuFireballActor.h"
#include "Monster/AttackActor/GnuFiretornadoActor.h"
#include "Monster/AttackActor/GnuFirebreathActor.h"
#include "Characters/GnuCharacter.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimMontage.h"


AGnuMonster::AGnuMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 컨트롤러의 회전 사용 여부
	bUseControllerRotationYaw = false;

	// 시간상 이렇게 hp를 구현하는게 맞긴한데........
}

void AGnuMonster::SpawnFireball()
{
	if (FireballClass)  // FireballClass는 BP에서 설정한 파이어볼 클래스
	{
		FVector HeadLoaction = GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		FVector SpawnLocation = HeadLoaction + GetActorForwardVector() * 100;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetActorRotation();

		// 파이어볼 액터를 스폰
		AGnuFireballActor* Fireball = GetWorld()->SpawnActor<AGnuFireballActor>(FireballClass, SpawnLocation, SpawnRotation);
		if (Fireball)
		{
			// 파이어볼을 발사하는 메서드 호출
			Fireball->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange,TEXT("Start Fireball LaunchProjectile"));
		}
	}
}

void AGnuMonster::SpawnFiretornado()
{
	if (FiretornadoClass)
	{
		FVector BaseSpawnLocation = GetActorLocation() + GetActorForwardVector() * 500;
		BaseSpawnLocation.Z = 0.f;	// 높이 0으로 만들어서 바닥 높이에서 생성되도록

		FVector SpawnLocation1 = BaseSpawnLocation; // 원래 위치
		FVector SpawnLocation2 = BaseSpawnLocation + FVector(0.f, 500.f, 0.f);  // 약간 오른쪽으로 이동
		FVector SpawnLocation3 = BaseSpawnLocation + FVector(0.f, -500.f, 0.f);  // 약간 왼쪽으로 이동


		FRotator SpawnRotation1 = GetActorRotation();
		
		FRotator SpawnRotation2 = SpawnRotation1;
		SpawnRotation2.Yaw -= 30;

		FRotator SpawnRotation3 = SpawnRotation1;
		SpawnRotation3.Yaw += 30;


		AGnuFiretornadoActor* Firetornado = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation1, SpawnRotation1);
		AGnuFiretornadoActor* Firetornado2 = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation2, SpawnRotation2);
		AGnuFiretornadoActor* Firetornado3 = GetWorld()->SpawnActor<AGnuFiretornadoActor>(FiretornadoClass, SpawnLocation3, SpawnRotation3);

		if (Firetornado)
		{
			Firetornado->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("Start Firetornado LaunchProjectile"));
		}
		
		if (Firetornado2)
		{
			Firetornado2->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("Start Firetornado2 LaunchProjectile"));
		}

		if (Firetornado3)
		{
			Firetornado3->LaunchProjectile(this);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Orange, TEXT("Start Firetornado3 LaunchProjectile"));
		}
	}
}

void AGnuMonster::SpawnFirebreath()
{
	if (FirebreathClass)
	{
		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("HeadSocket")) + FVector(0.0f, 0.0f, -50.f);
		FVector SpawnLocation = HeadLocation + GetActorForwardVector() * 50;  // 몬스터 앞에 생성
		FRotator HeadRotation = GetMesh()->GetSocketRotation(TEXT("HeadSocket"));


		/*AGnuFirebreathActor* Firebreath = GetWorld()->SpawnActor<AGnuFirebreathActor>(FirebreathClass, SpawnLocation, SpawnRotation);*/
		FirebreathActor = GetWorld()->SpawnActor<AGnuFirebreathActor>(FirebreathClass, SpawnLocation, HeadRotation);
	}
}


void AGnuMonster::BeginPlay()
{
	Super::BeginPlay();
}

void AGnuMonster::InitializeAnimInstance()
{
}


void AGnuMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// FirebreathActor의 위치 업데이트
	if (FirebreathActor)
	{
		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		FVector SpawnLocation = HeadLocation + GetActorForwardVector() * 50;  // 몬스터 앞에 생성
		FRotator SpawnRotation = GetMesh()->GetSocketRotation(TEXT("HeadSocket"));
		SpawnRotation.Pitch += 70;

		FirebreathActor->SetActorLocation(SpawnLocation + SpawnRotation.Vector() * 100); // 몬스터 앞쪽으로 이동
		FirebreathActor->SetActorRotation(SpawnRotation); // 몬스터와 같은 방향으로 회전
	}
}


void AGnuMonster::PlayDefaultAttackMontage()
{
	UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance != nullptr)
	{
		instance->Montage_Play(AttackMontage);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Play Attack Montage"));
	}
}

void AGnuMonster::PlayFlyingAttackMontage()
{
	UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance != nullptr)
	{
		instance->Montage_Play(FlyingAttackMontage);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Play Flying Attack Montage"));
	}
}

void AGnuMonster::PlayFirebreathAttackMontage()
{
	UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance != nullptr)
	{
		instance->Montage_Play(FirebreathAttackMontage);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Play Firebreath Attack Montage"));
	}
}