// Fill out your copyright notice in the Description page of Project Settings.


#include "AIDirector.h"

#include "AttackTerm.h"
#include "PlayerAdaptation.h"

// Sets default values for this component's properties
UAIDirector::UAIDirector()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

int32 UAIDirector::ChooseNextAttack(const FFightContext& Context)
{
	int32 HighestScoreId = 0;
	float HighestScore = -FLT_MAX;
	
	
	if (Model)
	{
		if (ValidAttacks.Num() > 0)
		{
			for (const auto& Attack : ValidAttacks)
			{
				const auto& Prediction = Model->Predict(FPlayerActionContext(Context, Attack.Id));
				float FinalScore = 0.f;
				if (AdaptationEnabled && Prediction.Confidence > Model->ConfidenceThreshold)
				{
					if (Attack.PunishableActions.Contains(Prediction.Action))
					{
						FinalScore += 10 * Prediction.Confidence;
					}
				}
				for (const auto& Tag  : Context.Tags)
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
					Model->UpdateCurrentPrediction(Prediction);
				}
			}
			Model->OpenTrackingWindow(FPlayerActionContext{.Combat = Context, .BossAction = HighestScoreId}, 1.5f /*Delay can be calculated or given by user*/);
		
			const auto* ChosenAttack = ValidAttacks.FindByPredicate(
		[HighestScoreId](const FBossAttack& A) { return A.Id == HighestScoreId; });
			AttacksOnCooldown.Add(*ChosenAttack);
			ValidAttacks.Remove(*ChosenAttack);
		}
	}
	return HighestScoreId;
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

	// ...
}

