// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/GnuMonster.h"
#include "Components/SphereComponent.h"
#include "Characters/GnuCharacter.h"


AGnuMonster::AGnuMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectRange = CreateDefaultSubobject<USphereComponent>(TEXT("DetectRange"));
	DetectRange->SetupAttachment(GetRootComponent());
}


void AGnuMonster::BeginPlay()
{
	Super::BeginPlay();

	// DetectRange Overlap Function Binding
	DetectRange->OnComponentBeginOverlap.AddDynamic(this, &AGnuMonster::OnPlayerEnterRange);
	DetectRange->OnComponentEndOverlap.AddDynamic(this, &AGnuMonster::OnPlayerExitRange);
}


void AGnuMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGnuMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Target BeginOverlap Function
void AGnuMonster::OnPlayerEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		AGnuCharacter* Target = Cast<AGnuCharacter>(OtherActor);
		if (Target && GetWorld())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, TEXT("BeginOverlap"));
			PlayAttackMontage();
		}
	}
}

// Target EndOverlap Function
void AGnuMonster::OnPlayerExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, TEXT("EndOverlap"));
}


void AGnuMonster::PlayAttackMontage()
{
	/*UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance != nullptr)
	{
		instance->Montage_Play(AttackMontage);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("PlayAttackMontage"));
	}*/
}