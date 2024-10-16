// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuMyCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystems/GnuAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/GnuCharacterPlayerController.h"
#include "Characters/GnuCharacterPlayerState.h"
#include "UI/GnuHUD.h"

AGnuMyCharacter::AGnuMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	///////////////////////////
	// 캐릭터 회전
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->RotationRate = FRotator(0.f, 400.f, 0.f);
	//////////////////////////
}

void AGnuMyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
}
void AGnuMyCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}
void AGnuMyCharacter::InitAbilityActorInfo()
{
	AGnuCharacterPlayerState* GnuPlayerState = GetPlayerState<AGnuCharacterPlayerState>();
	check(GnuPlayerState);
	GnuPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(GnuPlayerState, this);
	Cast<UGnuAbilitySystemComponent>(GnuPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = GnuPlayerState->GetAbilitySystemComponent();
	AttributeSet = GnuPlayerState->GetAttributeSet();

	if (AGnuCharacterPlayerController* GnuPlayerController = Cast<AGnuCharacterPlayerController>(GetController()))
	{
		if (AGnuHUD* GnuHUD = Cast<AGnuHUD>(GnuPlayerController->GetHUD()))
		{
			GnuHUD->InitOverlay(GnuPlayerController, GnuPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}