// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAdaptation.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "OpponentAdapatationCharacter.generated.h"

enum class EPlayerAction : uint8;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerActionRecorded, EPlayerAction, Action);

UENUM(BlueprintType)
enum class ECostActions : uint8
{
	Attack,
	Dodge,
	Roll,
	Run
};

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AOpponentAdapatationCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DashMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DodgeBackMontage;
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DodgeForwardMontage;
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DodgeRightMontage;
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage> DodgeLeftMontage;

public:
	/** Constructor */
	AOpponentAdapatationCharacter();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation")
	EPlayerAction CurrentPlayerAction;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Adaptation")
	FOnPlayerActionRecorded OnPlayerActionRecorded;

protected:
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Dodge(const FInputActionValue& Value);

	void Roll(const FInputActionValue& Value);

	void RunStart(const FInputActionValue& Value);
	void RunStop();
	void RunEnd(const FInputActionValue& Value);

public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Health = MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxStamina = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Stamina = MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Costs")
	float DodgeCost = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Costs")
	float RollCost = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Costs")
	float AttackCost = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Costs")
	float RunCost = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Costs")
	float RegainStaminaRate = 3.f;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsBusy = false;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsRunning = false;

	UFUNCTION(BlueprintCallable, Category = "Costs")
	bool StaminaCheck(const ECostActions ActionType);
};
