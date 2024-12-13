// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GnuProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

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
				GrenadeOnHit(OwnerController, HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
			}
			else
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			}
		}
	}


	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
}

void AGnuProjectileBullet::GrenadeOnHit(AController* ownerController, UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, FString("Hit"));
	//CollisionBox->SetSimulatePhysics(false);
	//CollisionBox->SetBoxExtent(FVector(300.0f, 300.0f, 300.0f), true);
	//CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//FVector HitLocation = Hit.Location + FVector(0.0f, 0.0f, 60.0f);
	//FRotator HitRotation = FRotator(UKismetMathLibrary::RandomFloatInRange(GetActorRotation().Roll, 0.0f), UKismetMathLibrary::RandomFloatInRange(GetActorRotation().Pitch,
	//	0.0f), UKismetMathLibrary::RandomFloatInRange(GetActorRotation().Yaw, 0.0f));

	//if (GrenadeCastNiagara)
	//{
	//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GrenadeCastNiagara, HitLocation, HitRotation);
	//}

	//if (GrenadeNiagara)
	//{
	//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GrenadeNiagara, HitLocation, GetActorRotation());
	//}

	UGameplayStatics::ApplyDamage(OtherActor, Damage, ownerController, this, UDamageType::StaticClass());

}
