// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_Combat/AI/EnemyRock.h"

AEnemyRock::AEnemyRock()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	StaticMeshComp->SetupAttachment(GetMesh());
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!StaticMeshComp) return;

	if (bIsAttacking)
	{
		switch (CurrentAttackType)
		{
		case EAttackType::HammerAttack:
			{
				ToppleAlpha = FMath::Min(ToppleAlpha + DeltaTime / ToppleDuration, 1.f);
				HammerAttack();
				if (ToppleAlpha >= 1.f)
				{
					CurrentState = EEnemyState::Toppled;
					bIsAttacking = false;
				}
				break;
			}
		case EAttackType::RollAttack:
			{
				ToppleAlpha = FMath::Min(ToppleAlpha + DeltaTime / ToppleDuration, 1.f);
				RollAttack();
				if (ToppleAlpha >= 1.f)
				{
					CurrentState = EEnemyState::Rolling;
					LaunchCharacter(GetActorForwardVector() * 5000.f, true, true);
					bIsAttacking = false;
				}
				break;
			}
		case EAttackType::CircleAttack:
			{
				break;
			}
		}
	}

	switch (CurrentState)
	{
	case EEnemyState::Standing:
		{
			break;
		}
	case EEnemyState::Toppled:
		{
			RiseAxis = FRotator(0.f, 0.f, 90.f);
			DelayTimer = 1.f;
			CurrentState = EEnemyState::Waiting;
			break;
		}
	case EEnemyState::Rising:
		{
			ToppleAlpha = FMath::Max(ToppleAlpha - DeltaTime / ToppleDuration, 0.f);
			const FRotator Current = FMath::Lerp(FRotator::ZeroRotator, RiseAxis, ToppleAlpha);
			StaticMeshComp->SetRelativeRotation(Current);
			if (ToppleAlpha <= 0.f)
			{
				CurrentState = EEnemyState::Standing;
			}
			break;
		}
	case EEnemyState::Rolling:
		{
			RiseAxis = FRotator(90.f, 0.f, 0.f);
			DelayTimer = 2.f;
			CurrentState = EEnemyState::Waiting;
			break;
		}
	case EEnemyState::Waiting:
		{
			DelayTimer -= DeltaTime;
			if (DelayTimer < 0.f)
			{
				CurrentState = EEnemyState::Rising;
				ToppleDuration = CurrentDelay * 0.5f;
			}
		}
	}
}

EEnemyState AEnemyRock::GetCurrentState() const
{
	return CurrentState;
}

void AEnemyRock::HammerAttack() const
{
	const FRotator Current = FMath::Lerp(FRotator::ZeroRotator, FRotator(0.f, 0.f, 90.f), ToppleAlpha);
	StaticMeshComp->SetRelativeRotation(Current);
}

void AEnemyRock::RollAttack() const
{
	const FRotator Current = FMath::Lerp(FRotator::ZeroRotator, FRotator(90.f, 0.f, 0.f), ToppleAlpha);
	StaticMeshComp->SetRelativeRotation(Current);
}
