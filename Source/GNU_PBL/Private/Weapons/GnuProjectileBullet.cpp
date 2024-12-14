// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Monster/GnuMonster.h"

void AGnuProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			if (GrenadeBeforeNiagara)
			{
				Multicast_DestroyChargeNiagaraComponent();
				CastGrenade(OwnerController, HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
				
			}
			else
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
			}
		}
	}
}

void AGnuProjectileBullet::CastGrenade(AController* ownerController, UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CollisionBox->SetBoxExtent(FVector(300.0f, 300.0f, 300.f), true);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌을 감지할 수 있게 설정
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);

	FVector HitLocation = Hit.Location + FVector(0.0f, 0.0f, 60.0f);
	FRotator HitRotation = FRotator(UKismetMathLibrary::RandomFloatInRange(GetActorRotation().Roll, 0.0f), UKismetMathLibrary::RandomFloatInRange(GetActorRotation().Pitch,
		0.0f), UKismetMathLibrary::RandomFloatInRange(GetActorRotation().Yaw, 0.0f));


	// 멀티캐스트로 이펙트를 모든 클라이언트에 스폰
	Multicast_CastGrenadeEffect(HitLocation, HitRotation);
	//if (GrenadeCastNiagara)
	//{
	//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GrenadeCastNiagara, HitLocation, HitRotation);
	//}


	GetWorld()->GetTimerManager().SetTimer(GrenadeBoomTimerHandle, FTimerDelegate::CreateUObject(this, &AGnuProjectileBullet::Multicast_BoomGrenadeEffect, ownerController, HitLocation, HitRotation), 1.0f, false);


	GetWorld()->GetTimerManager().SetTimer(DestoryTimer, FTimerDelegate::CreateLambda([this, HitComp, OtherActor, OtherComp, NormalImpulse, Hit]()
		{
			DestoryGrenade(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
		}), 2.5f, false);

}

void AGnuProjectileBullet::BoomGrenade(AController* ownerController, FVector HitLocation, FRotator HitRotation)
{
	if (GrenadeBoomNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GrenadeBoomNiagara, HitLocation, GetActorRotation());
		
		ApplyDamageInRange(ownerController);
	}
}

void AGnuProjectileBullet::ApplyDamageInRange(AController* ownerController)
{
	TArray<AActor*> OverlappingActors;
	CollisionBox->GetOverlappingActors(OverlappingActors);

	// 겹쳐 있는 각 액터에 대해 데미지를 적용
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->IsA(AGnuMonster::StaticClass()))
		{
			UGameplayStatics::ApplyDamage(Actor, Damage, ownerController, this, UDamageType::StaticClass());
		}
	}
}

void AGnuProjectileBullet::DestoryGrenade(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

// 멀티 구현
void AGnuProjectileBullet::Multicast_CastGrenadeEffect_Implementation(FVector HitLocation, FRotator HitRotation)
{
	if (GrenadeCastSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GrenadeCastSound, GetActorLocation(), 1.0f);
	}

	if (GrenadeCastNiagara)
	{
		GrenadeCastNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			GrenadeCastNiagara,
			CollisionBox,
			FName(),
			HitLocation,
			HitRotation,
			EAttachLocation::KeepWorldPosition,
			true
		);
	}
}

void AGnuProjectileBullet::Multicast_BoomGrenadeEffect_Implementation(AController* ownerController, FVector HitLocation, FRotator HitRotation)
{
	if (GrenadeBoomNiagara)
	{
		if(GrenadeCastNiagara)
			GrenadeCastNiagaraComponent->DestroyComponent();
		
		if (GrenadeBoomSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, GrenadeBoomSound, GetActorLocation(), 1.0f);
		}

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GrenadeBoomNiagara, HitLocation, GetActorRotation());

		ApplyDamageInRange(ownerController);
	}
}

void AGnuProjectileBullet::Multicast_DestroyChargeNiagaraComponent_Implementation()
{
	if (ChargeNiagaraComponent)
	{
		ChargeNiagaraComponent->DestroyComponent();
	}
}