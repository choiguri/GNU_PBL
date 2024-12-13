// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuCombatComponent.h"
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


UGnuCombatComponent::UGnuCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGnuCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGnuCombatComponent, EquippedWeapon);
}

void UGnuCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGnuCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrosshairs(DeltaTime);

	// �ǽð� LineTrace �� HitTarget�� �Ҵ�
	if (GnuCharacter && GnuCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
	}
}

// LineTrace
void UGnuCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
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
		// ī�޶�� ĳ������ �Ÿ��� ���ϱ�
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

void UGnuCombatComponent::SetHUDCrosshairs(float DeltaTime)
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

			// ���ڼ� ���� ���
			// Ex ĳ������ �����ӵ� 0, �ְ�ӵ� 500�̸�  => �����ӵ� 0, �ְ�ӵ� 1
			// 0�� 1������ ������ ��ȯ
			
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

void UGnuCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && GnuCharacter)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = GnuCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket"));

		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, GnuCharacter->GetMesh());
		}

		GnuCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		GnuCharacter->bUseControllerRotationYaw = true;
	}
}

void UGnuCombatComponent::FireButtonPressed(bool bPressed)
{
	if (EquippedWeapon == nullptr) return;

	if (GnuCharacter->IsPlayingReactMontage()) return;

	if (FireDelay != EquippedWeapon->Delay) FireDelay = EquippedWeapon->Delay;

	bFireButtonPressed = bPressed;

	if (GnuCharacter->isSprint)
	{
		GnuCharacter->ServerSprintEnd();
		//GnuCharacter->UpdateSprintState(false);
	}
	
	if (bFireButtonPressed && !bReloadButtonPressed)
	{
		if (CanFire())
		{
			bCanFire = false;
			ServerFire(HitTarget);
		}
		StartFireTimer();
	}
}

// Automatic Fire
void UGnuCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || GnuCharacter == nullptr) return;
	if (GnuCharacter->IsPlayingReactMontage()) return;
	if (GnuCharacter->isSprint)
	{
		GnuCharacter->ServerSprintEnd();
		//GnuCharacter->UpdateSprintState(false);
	}

	GnuCharacter->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UGnuCombatComponent::FireTimerFinished,
		FireDelay
	);
}

void UGnuCombatComponent::FireTimerFinished()
{
	bCanFire = true;
	if (GnuCharacter->IsPlayingReactMontage()) return;
	if (GnuCharacter->isSprint)
	{
		GnuCharacter->ServerSprintEnd();
		//GnuCharacter->UpdateSprintState(false);
	}
	if (bFireButtonPressed && bAutomatic)
	{
		if (CanFire())
		{
			bCanFire = false;
			ServerFire(HitTarget);
		}
		StartFireTimer();
	}
}

bool UGnuCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	return !EquippedWeapon->IsEmptyAmmo() || !bCanFire;
}

void UGnuCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}

void UGnuCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (GnuCharacter)
	{
		GnuCharacter->PlayFireMontage();
		EquippedWeapon->Fire(TraceHitTarget);
	}
}




void UGnuCombatComponent::ReloadButtonPressed(bool bPressed)
{
	bReloadButtonPressed = bPressed;
	if (EquippedWeapon == nullptr) return;
	if (GnuCharacter->IsPlayingReactMontage()) return;
	if (GnuCharacter && !bFireButtonPressed) // !bFireButtonPressed ������ ���� ��Ŭ��(�߻�)�� ���� �ʾƵ� �ٷ� ������ ����
	{
		EquippedWeapon->Reload();
		GnuCharacter->PlayReloadMontage();
	}
}

void UGnuCombatComponent::ReloadFinished(bool bPressed)
{
	if (EquippedWeapon == nullptr) return;
	EquippedWeapon->ReloadFinished();
	bReloadButtonPressed = bPressed;
}

// ���� ����
void UGnuCombatComponent::EquipWeapon(AGnuWeapon* WeaponToEquip)
{
	if (GnuCharacter == nullptr) return;

	// �̹� ���⸦ ���� ���̸� ���⸦ ������
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->Dropped();
		EquippedWeapon = nullptr;
	}
	// ���⸦ ������ �� Overlap�Ǿ��ִ� ���Ⱑ �ִٸ� ���� ��ü, �ƴϸ� ���� �ִ� ���� ������
	if (WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = GnuCharacter->GetMesh()->GetSocketByName(FName("WeaponSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, GnuCharacter->GetMesh());
	}

	EquippedWeapon->SetOwner(GnuCharacter);
	EquippedWeapon->UpdateAmmo();
}



