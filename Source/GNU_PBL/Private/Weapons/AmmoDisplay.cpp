// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/AmmoDisplay.h"

void UAmmoDisplay::UpdateAmmo(int RemainAmmo, int MaxAmmo)
{
	// 텍스트를 "남은탄약 / 최대탄약" 형태로 업데이트
	FString AmmoText = FString::Printf(TEXT("%d / %d"), RemainAmmo, MaxAmmo);
	Ammo->SetText(FText::FromString(AmmoText));
}

void UAmmoDisplay::DisplayAmmo(int RemainAmmo, int MaxAmmo)
{

}
