// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ReloadComplate.h"
#include "Characters/GnuMyCharacter.h" // 당신의 캐릭터 클래스를 포함합니다.

void UReloadComplate::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGnuMyCharacter* Character = Cast<AGnuMyCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		// 특정 동작을 실행합니다.
		Character->Reload(); // 호출하고자 하는 함수로 변경하세요.
	}
}
