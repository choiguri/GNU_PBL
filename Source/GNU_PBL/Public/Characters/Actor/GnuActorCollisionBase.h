// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GnuActorCollisionBase.generated.h"



UCLASS()
class GNU_PBL_API AGnuActorCollisionBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGnuActorCollisionBase();

protected:

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxComponent;

    UPROPERTY(VisibleAnywhere)
    UArrowComponent* ArrowComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};