// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpponentAdapatationCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "OpponentAdapatation.h"
#include "PlayerAdaptation.h"

AOpponentAdapatationCharacter::AOpponentAdapatationCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AOpponentAdapatationCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this,
		                                   &AOpponentAdapatationCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &AOpponentAdapatationCharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &AOpponentAdapatationCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this,
		                                   &AOpponentAdapatationCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &AOpponentAdapatationCharacter::Look);

		//Dodge
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this,
		                                   &AOpponentAdapatationCharacter::Dodge);

		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this,
		                                   &AOpponentAdapatationCharacter::Roll);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this,
		                                   &AOpponentAdapatationCharacter::RunStart);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this,
		                                   &AOpponentAdapatationCharacter::RunEnd);
	}
	else
	{
		UE_LOG(LogOpponentAdapatation, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AOpponentAdapatationCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsRunning)
	{
		if (const float NewStamina = Stamina - DeltaTime * RunCost; NewStamina > 0)
		{
			Stamina = NewStamina;
		}
		else
		{
			RunStop();
			Stamina = 0;
		}
	}
	else
	{
		if (const float NewStamina = Stamina + DeltaTime * RegainStaminaRate; NewStamina < MaxStamina)
		{
			Stamina = NewStamina;
		}
	}
}

void AOpponentAdapatationCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AOpponentAdapatationCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AOpponentAdapatationCharacter::Dodge(const FInputActionValue& Value)
{
	if (GetController() != nullptr && !bIsBusy)
	{
		if (StaminaCheck(ECostActions::Dodge)) return;
		bIsBusy = true;

		FVector Direction = GetLastMovementInputVector();
		if (Direction.IsNearlyZero())
		{
			Direction = -GetActorForwardVector();
		}
		Direction.Normalize();

		const FVector Local = GetActorTransform().InverseTransformVectorNoScale(Direction);
		if (FMath::Abs(Local.Y) > FMath::Abs(Local.X))
		{
			if (Local.Y > 0)
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::DodgeRight);
				PlayAnimMontage(DodgeRightMontage);
			}
			else
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::DodgeLeft);
				PlayAnimMontage(DodgeLeftMontage);
			}
		}
		else
		{
			if (Local.X > 0)
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::DodgeForward);
				PlayAnimMontage(DodgeForwardMontage);
			}
			else
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::DodgeBack);
				PlayAnimMontage(DodgeBackMontage);
			}
		}
	}
}

void AOpponentAdapatationCharacter::Roll(const FInputActionValue& Value)
{
	if (GetController() != nullptr && !bIsBusy)
	{
		if (!StaminaCheck(ECostActions::Roll)) return;
		bIsBusy = true;

		FVector Direction = GetLastMovementInputVector();
		if (Direction.IsNearlyZero())
		{
			Direction = -GetActorForwardVector();
		}
		Direction.Normalize();

		SetActorRotation(Direction.Rotation());
		PlayAnimMontage(DashMontage);

		const FVector Local = GetActorTransform().InverseTransformVectorNoScale(Direction);
		if (FMath::Abs(Local.Y) > FMath::Abs(Local.X))
		{
			if (Local.Y > 0)
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::RollRight);
			}
			else
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::RollLeft);
			}
		}
		else
		{
			if (Local.X > 0)
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::RollForward);
			}
			else
			{
				OnPlayerActionRecorded.Broadcast(EPlayerAction::RollBack);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("input=%s  dir=%s  local=%s  actorYaw=%.1f  ctrlYaw=%.1f"),
		       *GetLastMovementInputVector().ToString(),
		       *Direction.ToString(),
		       *Local.ToString(),
		       GetActorRotation().Yaw,
		       GetControlRotation().Yaw);
	}
}

void AOpponentAdapatationCharacter::RunStart(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	bIsRunning = true;
}

void AOpponentAdapatationCharacter::RunStop()
{
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	bIsRunning = false;
}

void AOpponentAdapatationCharacter::RunEnd(const FInputActionValue& Value)
{
	RunStop();
}

void AOpponentAdapatationCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr && !bIsBusy)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);

		// Broadcasting direction
		const FVector WorldDirection = (ForwardDirection * Forward) + (RightDirection * Right);
		const FVector Local = GetActorTransform().InverseTransformVectorNoScale(WorldDirection);
	}
}

void AOpponentAdapatationCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AOpponentAdapatationCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
	OnPlayerActionRecorded.Broadcast(EPlayerAction::Jump);
}

void AOpponentAdapatationCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

bool AOpponentAdapatationCharacter::StaminaCheck(const ECostActions ActionType)
{
	switch (ActionType)
	{
	case ECostActions::Attack:
		{
			float NewStamina = Stamina - AttackCost;
			if (NewStamina > 0)
			{
				Stamina = NewStamina;
				return true;
			}
			return false;
		}
	case ECostActions::Roll:
		{
			float NewStamina = Stamina - RollCost;
			if (NewStamina > 0)
			{
				Stamina = NewStamina;
				return true;
			}
			return false;
		}
	case ECostActions::Dodge:
		{
			float NewStamina = Stamina - DodgeCost;
			if (NewStamina > 0)
			{
				Stamina = NewStamina;
				return true;
			}
			return false;
		}
	default:
		return false;
	}
}
