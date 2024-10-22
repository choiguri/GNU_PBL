// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/GnuRollNotify.h"
#include "Characters/GnuMyCharacter.h"

void UGnuRollNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    AGnuMyCharacter* Character = Cast<AGnuMyCharacter>(MeshComp->GetOwner());
    if (Character)
    {
        if (Character->isDodge == true)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("isDodging : true에서 False"));
            }
            Character->isDodge = false;
        }
        else if (Character->isDodge == false)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("isDodging : false 에서 true"));
            }
            Character->isDodge = true;
 
        } 
    }
}
