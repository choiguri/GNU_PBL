// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonster.h"
#include "Monster/GnuFireballActor.h"
#include "Characters/GnuCharacter.h"


AGnuMonster::AGnuMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange,TEXT("Start LaunchProjectile"));
		}
	}
}


void AGnuMonster::BeginPlay()
{
	Super::BeginPlay();
}


void AGnuMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGnuMonster::PlayAttackMontage()
{
	UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance != nullptr)
	{
		instance->Montage_Play(AttackMontage);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("PlayAttackMontage"));
	}
}