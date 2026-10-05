// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpponentAdapatationGameMode.h"

#include "CombatAIController.h"
#include "CombatEnemy.h"
#include "OpponentAdapatationCharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "Kismet/GameplayStatics.h"

AOpponentAdapatationGameMode::AOpponentAdapatationGameMode()
{
	// stub
}

void AOpponentAdapatationGameMode::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AOpponentAdapatationCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AOpponentAdapatationCharacter::StaticClass()));
	Boss = Cast<ACombatEnemy>(UGameplayStatics::GetActorOfClass(GetWorld(), ACombatEnemy::StaticClass()));
	PlayerStartLocation = Player->GetActorLocation();
	PlayerStartRotation = Player->GetActorRotation();
	BossStartLocation = Boss->GetActorLocation();
	BossStartRotation = Boss->GetActorRotation();
}

void AOpponentAdapatationGameMode::ResetFight() const
{
	// boss
	Boss->SetActorLocation(BossStartLocation);
	Boss->SetActorRotation(BossStartRotation);
	Boss->CurrentHP = Boss->MaxHP;
	Cast<ACombatAIController>(Boss->GetController())->Reset();
	Cast<ACombatAIController>(Boss->GetController())->GetStateTree()->RestartLogic();
	
	auto* AdaptationModel = Cast<UPlayerAdaptation>(Boss->GetComponentByClass(UPlayerAdaptation::StaticClass()));
	if (Player->Health > 0.f)
	{
		AdaptationModel->HardReset();
	}
	else
	{
		AdaptationModel->SoftReset();
	}

	// player
	Player->SetActorLocation(PlayerStartLocation);
	Player->Health = Player->MaxHealth;
	Player->Stamina = Player->MaxStamina;
}
