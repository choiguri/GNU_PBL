// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Characters/GnuMyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/GnuMyPlayerController.h"
#include "Weapons/GnuCombatComponent.h"


AGnuWeapon::AGnuWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(Root);
	/*SetRootComponent(WeaponMesh);*/

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(WeaponMesh);
	/*AreaSphere->SetupAttachment(RootComponent);*/
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(WeaponMesh);
	/*PickupWidget->SetupAttachment(RootComponent);*/

	bisReload = false;
}

void AGnuWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AGnuWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AGnuWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	Delay = 60 / RPM;

	InitialRelativeLocation = WeaponMesh->GetRelativeLocation();
	InitialRelativeRotation = WeaponMesh->GetRelativeRotation();

}

void AGnuWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetRecoil(DeltaTime);
}

void AGnuWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGnuWeapon, WeaponState);
	DOREPLIFETIME(AGnuWeapon, Ammo);

	DOREPLIFETIME(AGnuWeapon, InitialRelativeLocation);
	DOREPLIFETIME(AGnuWeapon, InitialRelativeRotation);


}

void AGnuWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedCompoonent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGnuMyCharacter* GnuCharacter = Cast<AGnuMyCharacter>(OtherActor);
	if (GnuCharacter)
	{
		GnuCharacter->SetOverlappingWeapon(this);
	}
}

void AGnuWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedCompoonent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGnuMyCharacter* GnuCharacter = Cast<AGnuMyCharacter>(OtherActor);
	if (GnuCharacter)
	{
		GnuCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AGnuWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AGnuWeapon::OnRep_Ammo()
{
	GnuOwnerCharacter = GnuOwnerCharacter == nullptr ? Cast<AGnuMyCharacter>(GetOwner()) : GnuOwnerCharacter;
	UpdateAmmo();
}

void AGnuWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		GnuOwnerCharacter = nullptr;
		GnuOwnerController = nullptr;
	}
	else
	{
		UpdateAmmo();
	}
}

void AGnuWeapon::SpendAmmo()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MaxAmmo);
	UpdateAmmo();
}

void AGnuWeapon::UpdateAmmo()
{
	GnuOwnerCharacter = GnuOwnerCharacter == nullptr ? Cast<AGnuMyCharacter>(GetOwner()) : GnuOwnerCharacter;
	if (GnuOwnerCharacter)
	{
		GnuOwnerController = GnuOwnerController == nullptr ? Cast<AGnuMyPlayerController>(GnuOwnerCharacter->Controller) : GnuOwnerController;
		if (GnuOwnerController)
		{
			GnuOwnerController->SetHUDWeaponAmmo(Ammo, MaxAmmo);
		}
	}
}

bool AGnuWeapon::IsEmptyAmmo()
{
	return Ammo <= 0;
}

void AGnuWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped :
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		WeaponMesh->SetRelativeLocationAndRotation(InitialRelativeLocation, InitialRelativeRotation);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AGnuWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		WeaponMesh->SetRelativeLocationAndRotation(InitialRelativeLocation, InitialRelativeRotation);
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AGnuWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	Root->DetachFromComponent(DetachRules);

	SetOwner(nullptr);
	GnuOwnerCharacter = nullptr;
	GnuOwnerController = nullptr;
}

void AGnuWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	SpendAmmo();
}

void AGnuWeapon::Reload()
{
	if (ReloadAnimation)
	{
		WeaponMesh->PlayAnimation(ReloadAnimation, false);
	}
	bisReload = true;
}

void AGnuWeapon::ReloadFinished()
{
	Ammo = MaxAmmo;
	bisReload = false;
	UpdateAmmo();
}

void AGnuWeapon::SetRecoil(float DeltaTime)
{
	GnuOwnerCharacter = GnuOwnerCharacter == nullptr ? Cast<AGnuMyCharacter>(GetOwner()) : GnuOwnerCharacter;
	if (GnuOwnerCharacter)
	{
		if (GnuOwnerCharacter->Combat->bFireButtonPressed && !IsEmptyAmmo())
		{
			GnuOwnerCharacter->AddControllerPitchInput(FMath::RandRange(-0.03f, 0.f));
			GnuOwnerCharacter->AddControllerYawInput(FMath::RandRange(-0.1f, 0.1f));
		}
	}
}

USkeletalMeshComponent* AGnuWeapon::GetMesh()
{
	return WeaponMesh;
}

void AGnuWeapon::OnRep_InitialTransform()
{
	WeaponMesh->SetRelativeLocationAndRotation(InitialRelativeLocation, InitialRelativeRotation);
}
