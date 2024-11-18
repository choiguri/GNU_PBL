// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CombatComponent.h"
#include "Weapons/GnuWeapon.h"
#include "Characters/GnuMyCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Characters/GnuMyPlayerController.h"
#include "HUD/GNUHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"


UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrosshairs(DeltaTime);

	// 실시간 LineTrace 값 HitTarget에 할당
	if (GnuCharacter && GnuCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
	}
}


// LineTrace
void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		// 카메라와 캐릭터의 거리를 구하기
		if (GnuCharacter)
		{
			float DistanceToCharacter = (GnuCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 70.f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (GnuCharacter == nullptr || GnuCharacter->Controller == nullptr) return;
	
	Controller = Controller == nullptr ? Cast<AGnuMyPlayerController>(GnuCharacter->Controller) : Controller;
	if (Controller)
	{
		
		HUD = HUD == nullptr ? Cast<AGNUHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			FHUDPackage HUDPackage;
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairCenter;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
				HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;
			}
			else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
			}

			// 십자선 퍼짐 계산
			// Ex 캐릭터의 최저속도 0, 최고속도 500이면  => 최저속도 0, 최고속도 1
			// 0과 1사이의 값으로 변환
			
			FVector2D WalkSpeedRange(0.f, GnuCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = GnuCharacter->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (GnuCharacter->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}
			if (bFireButtonPressed)
			{
				CrosshairInFireFactor = FMath::FInterpTo(CrosshairInFireFactor, 1.25f, DeltaTime, 1.25f);
			}
			else
			{
				CrosshairInFireFactor = FMath::FInterpTo(CrosshairInFireFactor, 0.f, DeltaTime, 30.f);
			}

			HUDPackage.CrosshairSpread = CrosshairVelocityFactor + CrosshairInAirFactor + CrosshairInFireFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && GnuCharacter)
	{
		GnuCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		GnuCharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed)
	{
		if (bCanFire)
		{
			bCanFire = false;
			ServerFire(HitTarget);
		}
		StartFireTimer();
	}

}

// Automatic Fire
void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || GnuCharacter == nullptr) return;

	GnuCharacter->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	bCanFire = true;
	if (bFireButtonPressed && bAutomatic)
	{
		if (bCanFire)
		{
			bCanFire = false;
			ServerFire(HitTarget);
		}
		StartFireTimer();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}

void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (GnuCharacter)
	{
		GnuCharacter->PlayFireMontage();
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

// 무기 장착
void UCombatComponent::EquipWeapon(AGnuWeapon* WeaponToEquip)
{
	if (GnuCharacter == nullptr || WeaponToEquip == nullptr) return;
	if (EquippedWeapon != nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = GnuCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, GnuCharacter->GetMesh());
	}

	EquippedWeapon->SetOwner(GnuCharacter);

	
}

