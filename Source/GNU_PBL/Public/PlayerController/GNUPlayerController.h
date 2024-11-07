// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "InputActionValue.h"

#include "GNUPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GNU_PBL_API AGNUPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDCombatTime(float CombatTime);

	virtual void Tick(float DeltaTime) override;
	virtual float GetServertime();
	virtual void ReceivedPlayer() override;

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	class UInputAction* QuitAction;

	void ShowReturnToMainMenu();

	// 서버와 클라이언트 사이의 시간 동기화

	// 현재 서버 시간 요청
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// 현재 서버 시간과 서버에서 클라이언트로의 응답시간 
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // 서버와 클라이언트 사이의 차이 

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.0f;
	void CheckTimeSync(float DeltaTime);

	void HighPingWarning();
	void StopHighPingWarning();


private:
	UPROPERTY()
	class AGNUHUD* GNUHUD;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UGNUReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

	float TotalTime = 120.f;
	uint32 CountdownInt = 0;

	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;

	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;
};
