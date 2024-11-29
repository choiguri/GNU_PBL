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
	SetRootComponent(WeaponMesh);

	// �浹 ����
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ĳ���Ϳ� ��ġ�� �κ� ���� �ֿ� �� ����� Sphere
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	/*AreaSphere->SetupAttachment(WeaponMesh);*/
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// �ֿ� �� ������ Ű ���� ����
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	/*PickupWidget->SetupAttachment(WeaponMesh);*/
	PickupWidget->SetupAttachment(RootComponent);
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

void AGnuWeapon::Reload()
{
	if (ReloadAnimation)
	{
		WeaponMesh->PlayAnimation(ReloadAnimation, false);
	}
	UpdateAmmo();
	
}

bool AGnuWeapon::IsEmptyAmmo()
{
	return Ammo <= 0;
}

void AGnuWeapon::ReloadFinished()
{
	Ammo = MaxAmmo;
	UpdateAmmo();
}


void AGnuWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped :
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		WeaponMesh->SetRelativeLocationAndRotation(InitialRelativeLocation, InitialRelativeRotation);
		break;
	case EWeaponState::EWS_Dropped :
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}


void AGnuWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

USkeletalMeshComponent* AGnuWeapon::GetMesh()
{
	return WeaponMesh;
}

void AGnuWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	SpendAmmo();
}

void AGnuWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	Root->DetachFromComponent(DetachRules);

	SetOwner(nullptr);
}

void AGnuWeapon::OnRep_InitialTransform()
{
	WeaponMesh->SetRelativeLocationAndRotation(InitialRelativeLocation, InitialRelativeRotation);
}



void AGnuWeapon::SetRecoil(float DeltaTime)
{

	GnuOwnerCharacter = GnuOwnerCharacter == nullptr ? Cast<AGnuMyCharacter>(GetOwner()) : GnuOwnerCharacter;
	if (GnuOwnerCharacter)
	{
		if (GnuOwnerCharacter->Combat->bFireButtonPressed)
		{
			GnuOwnerCharacter->AddControllerPitchInput(FMath::RandRange(-0.03f, 0.f));
			GnuOwnerCharacter->AddControllerYawInput(FMath::RandRange(-0.1f, 0.1f));
		}
		
	}
}

