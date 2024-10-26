// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Monster/GAS/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "Monster/GAS/DataAsset_EnemyStartUpData.h"

AGnuEnemyCharacter::AGnuEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
}

void AGnuEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitEnemyStartUpData();
}

void AGnuEnemyCharacter::InitEnemyStartUpData()
{

	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	// 자산 관리자 함수 중에서 사용가능한 비동기적 로드를 위한 코드
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda( //람다 함수로 이름 없는 함수 선언으로 Delegate 생성
			[this]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(EnemyAbilitySystemComponent);
					GEngine->AddOnScreenDebugMessage(1, 2, FColor::Yellow, TEXT("Enemy Start Up Data Loaded"));
				}
			}
		)
	);
}
