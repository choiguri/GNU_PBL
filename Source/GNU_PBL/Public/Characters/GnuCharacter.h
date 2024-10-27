// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerState.h"
#include "GnuCharacter.generated.h"

// Input ����
class UInputMappingContext;
class UInputAction;
class UGnuCharacterAnimInstance;
// ��Ʈ�� ����
class USpringArmComponent;
class UCameraComponent;


UCLASS()
class GNU_PBL_API AGnuCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	UGnuCharacterAnimInstance* MyAnimInstance;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* RotationAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	UInputAction* JumpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	bool isWaking;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& value);
	void Rotation(const FInputActionValue& value);
	void UpdateAnimInstance(const FVector2D& MoveVector2D);

public:	
	// Sets default values for this character's properties
	AGnuCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


////////////////////
//////////////////// UI 파트
////////////////////
private:
	// 머리 위의 스팀 닉네임 표시
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverHeadWidget;

	// HP바 표시
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 1.f;

	UFUNCTION()
	void OnRep_Health();

	class AGNUPlayerController* GNUPlayerController;

	UPROPERTY(EditAnywhere, Category = "Player Name")
	FString LocalPlayerName = TEXT("Unknown Player");

public:
	// 서버에서 수정 -> RepNotify -> 클라이언트 반응
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Implementation으로 정의해서 밑줄이 뜨더라도 오류가 아님
	UFUNCTION(Client, Reliable)
	void ClientSetName(const FString& Name);

	UFUNCTION(Server, Reliable)
	void ServerSetPlayerName(const FString& PlayerName);

	//
	// GNUGameMode와 관련
	//
	void Elim();
};
