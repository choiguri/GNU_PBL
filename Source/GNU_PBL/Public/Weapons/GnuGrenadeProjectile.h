//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Weapons/GnuProjectile.h"
//#include "GnuGrenadeProjectile.generated.h"
//
//
//UCLASS()
//class GNU_PBL_API AGnuGrenadeProjectile : public AGnuProjectile
//{
//	GENERATED_BODY()
//	
//private:
//	FTimerHandle GrenadeTimer;
//
//	UFUNCTION()
//	void Boom();
//
//
//	//// Server function to handle grenade spawn
//	//UFUNCTION(Server, Reliable)
//	//void Server_SpawnGrenade(FVector Location, FRotator Rotation, ACharacter* OwnerCharacter);
//
//	//// MultiCast function to sync grenade spawn across all clients
//	//UFUNCTION(NetMulticast, Reliable)
//	//void Multicast_SpawnGrenade(FVector Location, FRotator Rotation);
//
//	//UFUNCTION(Client, Reliable) 
//	//void Client_SpawnGrenade(FVector Location, FRotator Rotation, ACharacter* OwnerCharacter);
//
//
//	//void SpawnGrenade(FVector Location, FRotator Rotation, ACharacter* OwnerCharacter);
//
//protected:
//	virtual void BeginPlay() override;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
//	TSubclassOf<AActor> BP_Grenade;
//
//	//UPROPERTY(EditAnywhere)
//	//class UNiagaraComponent* ChargeNiagaraComponent;
//
//	//UPROPERTY(EditAnywhere)
//	//class UNiagaraSystem* ChargeNiagara;
//
//};
