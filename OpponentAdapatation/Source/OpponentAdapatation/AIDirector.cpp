// Fill out your copyright notice in the Description page of Project Settings.


#include "AIDirector.h"

#include "RuleSet.h"
#include "PlayerAdaptation.h"

// Sets default values for this component's properties
UAIDirector::UAIDirector()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

const FBossAttack UAIDirector::ChooseNextAttack(const FGameplayTagContainer& Context)
{
	if (Model)
	{
		if (ValidAttacks.Num() > 0)
		{
			int32 HighestScoreId = 0;
			float HighestScore = -FLT_MAX;
			for (const auto& Attack : ValidAttacks)
			{
				const auto& ContextPrediction = Model->PredictBasedOnContext(FPlayerActionContext(Context, Attack.Id));
				const auto& AttackPrediction = Model->PredictBasedOnContext(FPlayerActionContext(Context, Attack.Id));
				float FinalScore = 0.f;
				if (AdaptationEnabled)
				{
					if (ContextPrediction.Confidence > Model->ConfidenceThreshold && Attack.PunishableActions.Contains(
						ContextPrediction.Action))
					{
						FinalScore += ContextBonus * ContextPrediction.Confidence;
					}
					if (!FightContextOnly && AttackPrediction.Confidence > Model->ConfidenceThreshold && ContextPrediction ==
						AttackPrediction)
					{
						FinalScore += AttackBonus * AttackPrediction.Confidence;
					}
				}

				for (const auto& Tag : Context)
				{
					if (const float* Found = Attack.Weights.Find(Tag))
					{
						FinalScore += *Found;
					}
				}
				if (FinalScore > HighestScore)
				{
					HighestScore = FinalScore;
					HighestScoreId = Attack.Id;
					Model->UpdateCurrentPrediction(ContextPrediction);
				}
			}
			
			Model->OpenTrackingWindow(FPlayerActionContext{.Tags = Context, .BossAction = HighestScoreId}, RecordingWindowTime);

			const auto* Found = ValidAttacks.FindByPredicate(
				[HighestScoreId](const FBossAttack& A) { return A.Id == HighestScoreId; });
			if (Found)
			{
				const FBossAttack ChosenAttack = *Found;
				AttacksOnCooldown.Add(ChosenAttack);
				ValidAttacks.Remove(ChosenAttack);
				return ChosenAttack;
			}
		}
	}
	return FBossAttack{};
}

float UAIDirector::CalculateFinalDelay(const FGameplayTagContainer& Context) const
{
	const float Multiplier = Rule->Score_Implementation(Context);
	return BaseDelay * Multiplier;
}

int32 UAIDirector::IdToIndex(int32 Id) const
{
	for (int i = 0; i < ValidAttacks.Num(); ++i)
	{
		if (ValidAttacks[i].Id == Id)
		{
			return i;
		}
	}
	return -1;
}


// Called when the game starts
void UAIDirector::BeginPlay()
{
	Super::BeginPlay();
	Model = GetOwner()->FindComponentByClass<UPlayerAdaptation>();
}


// Called every frame
void UAIDirector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int32 i = AttacksOnCooldown.Num() - 1; i >= 0; --i)
	{
		FBossAttack& Attack = AttacksOnCooldown[i];
		Attack.Timer += DeltaTime;

		if (Attack.Timer >= Attack.Cooldown)
		{
			Attack.Timer = 0.f;
			ValidAttacks.Add(Attack);
			AttacksOnCooldown.RemoveAt(i);
		}
	}
}
