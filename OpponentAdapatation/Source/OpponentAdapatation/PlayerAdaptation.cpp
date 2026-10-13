// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAdaptation.h"

// Sets default values for this component's properties
UPlayerAdaptation::UPlayerAdaptation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerAdaptation::OpenTrackingWindow(const FPlayerActionContext& Context, float WindowSeconds)
{
	// A new attack while a window is still open means the player never responded to the last
	// one. Close it so that non-response is recorded rather than silently dropped.
	if (bIsWindowOpen)
	{
		CloseTrackingWindow();
	}

	bIsWindowOpen = true;
	CurrentContext = Context;
	WindowTime = WindowSeconds;
}

void UPlayerAdaptation::RecordPlayerAction(EPlayerAction Action)
{
	if (!bIsWindowOpen)
	{
		return;
	}

	GradePrediction(Action);

	bIsWindowOpen = false;
	FActionCounts& ContextActionRef = TableOfContext.FindOrAdd(CurrentContext.Tags);
	FActionCounts& AttackActionRef = TableOfAttacks.FindOrAdd(CurrentContext.BossAction);


	// Decay every existing count before adding the new observation, so recent behavior
	// outweighs old.
	//
	// Decay does not reduce confidence by itself. Confidence only falls when
	// the player actually changes what they do.
	for (float& Weight : ContextActionRef.Weights)
	{
		Weight *= DecayPerNewContext;
	}

	for (float& Weight : AttackActionRef.Weights)
	{
		Weight *= DecayPerNewContext;
	}

	ContextActionRef.Total *= DecayPerNewContext;
	AttackActionRef.Total *= DecayPerNewContext;

	ContextActionRef.Weights[static_cast<int32>(Action)] += 1.f;
	ContextActionRef.Total += 1.f;

	AttackActionRef.Weights[static_cast<int32>(Action)] += 1.f;
	AttackActionRef.Total += 1.f;

	LogCurrentContext();
	LogPrediction();
	LogAccuracy();
}

void UPlayerAdaptation::LogTable() const
{
	UE_LOG(LogTemp, Log, TEXT("Player Adaptation Table:"));
	for (const auto& Pair : TableOfContext)
	{
		const FActionCounts& Counts = Pair.Value;
		FGameplayTagContainer Context = Pair.Key;
		UE_LOG(LogTemp, Log, TEXT("Context: %s"), *Context.ToString());
		for (int32 i = 0; i < static_cast<int32>(EPlayerAction::Count); ++i)
		{
			EPlayerAction Action = static_cast<EPlayerAction>(i);
			float Weight = Counts.Weights[i];
			float Confidence = 0;
			UE_LOG(LogTemp, Log, TEXT("  Action: %s, Weight: %.2f"), *UEnum::GetValueAsString(Action), Weight);
		}
	}
	UE_LOG(LogTemp, Log, TEXT(""));
}

void UPlayerAdaptation::LogCurrentContext() const
{
	UE_LOG(LogTemp, Log, TEXT("Current context Table:"));
	const auto& History = TableOfContext.Find(CurrentContext.Tags);
	if (TableOfContext.Contains(CurrentContext.Tags))
	{
		UE_LOG(LogTemp, Log, TEXT("Context: %s"), *CurrentContext.ToString());
		for (int32 i = 0; i < static_cast<int32>(EPlayerAction::Count); ++i)
		{
			EPlayerAction Action = static_cast<EPlayerAction>(i);
			const float Weight = History->Weights[i];
			UE_LOG(LogTemp, Log, TEXT("  Action: %s, Weight: %.2f"), *UEnum::GetValueAsString(Action), Weight);
		}

		UE_LOG(LogTemp, Log, TEXT(""));
	}
}

void UPlayerAdaptation::LogPrediction() const
{
	UE_LOG(LogTemp, Log, TEXT("Current Prediction: Action: %s, Confidence: %.2f"),
	       *UEnum::GetValueAsString(CurrentPrediction.Action), CurrentPrediction.Confidence);
}

void UPlayerAdaptation::LogAccuracy() const
{
	UE_LOG(LogTemp, Log, TEXT("Prediction Accuracy: %.2f%% (%d/%d)"), Accuracy * 100.f,
	       static_cast<int32>(TotalCorrectPredictions), static_cast<int32>(TotalPredictions));
}

void UPlayerAdaptation::GradePrediction(EPlayerAction Action)
{
	// Only predictions the boss would actually have acted on are graded. Scoring predictions
	// below the threshold would measure a model that was never used, and drag the figure down.
	if (CurrentPrediction.Confidence > ConfidenceThreshold)
	{
		TotalPredictions += 1.f;
		if (Action == CurrentPrediction.Action)
		{
			TotalCorrectPredictions += 1.f;
			UE_LOG(LogTemp, Log, TEXT("Prediction correct! Action: %s, Confidence: %.2f"),
			       *UEnum::GetValueAsString(Action), CurrentPrediction.Confidence);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Prediction incorrect. Predicted: %s, Actual: %s, Confidence: %.2f"),
			       *UEnum::GetValueAsString(CurrentPrediction.Action), *UEnum::GetValueAsString(Action),
			       CurrentPrediction.Confidence);
		}
	}

	Accuracy = TotalPredictions > 0 ? TotalCorrectPredictions / TotalPredictions : 0.f;
}

FPredictionResult UPlayerAdaptation::GetCurrentPrediction() const
{
	return CurrentPrediction;
}

FString UPlayerAdaptation::GetContextAsString() const
{
	return CurrentContext.ToString();
}

float UPlayerAdaptation::GetAccuracy() const
{
	return Accuracy;
}

// Called when the game starts
void UPlayerAdaptation::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UPlayerAdaptation::CloseTrackingWindow()
{
	// Player did not respond to attack therefore, closing the window by recording action None.
	RecordPlayerAction(EPlayerAction::None);
	bIsWindowOpen = false;

	LogCurrentContext();
	LogPrediction();
	LogAccuracy();
}

void UPlayerAdaptation::HardReset()
{
	SoftReset();
	TableOfContext.Reset();
	TableOfAttacks.Reset();
}

void UPlayerAdaptation::SoftReset()
{
	CurrentConfidence = 0.f;
	CurrentPrediction = FPredictionResult(EPlayerAction::None, 0.f);
	CurrentContext = FPlayerActionContext();
	TotalCorrectPredictions = 0.f;
	TotalPredictions = 0.f;
	Accuracy = 0.f;
}

FPredictionResult UPlayerAdaptation::PredictBasedOnContext(const FPlayerActionContext& Context)
{
	int32 BestIndex = 0;
	float BestWeight = 0.f;
	FPredictionResult NewPrediction;
	// Return none if the context doesnt exist in the table.
	if (!TableOfContext.Contains(Context.Tags))
	{
		NewPrediction = FPredictionResult(EPlayerAction::None, 0.f);
		return NewPrediction;
	}
	// Find the highest weight.
	auto& ActionHistory = TableOfContext[Context.Tags];
	auto& AllWeights = ActionHistory.Weights;
	for (int32 Index = 0; Index < static_cast<int32>(EPlayerAction::Count); Index++)
	{
		auto& CurrentWeight = AllWeights[Index];
		if (CurrentWeight > BestWeight)
		{
			BestWeight = CurrentWeight;
			BestIndex = Index;
		}
	}
	// Scale and adjust confidence when the samples are lower than MinSampleConfidence.
	const float SampleScale = FMath::Clamp(
		ActionHistory.Total / FMath::Max(static_cast<float>(MinSamplesForConfidence), 1.f), 0.f, 1.f);

	NewPrediction = FPredictionResult(static_cast<EPlayerAction>(BestIndex),
	                                  (BestWeight / ActionHistory.Total) * SampleScale);
	return NewPrediction;
}

FPredictionResult UPlayerAdaptation::PredictBasedOnAttack(const FPlayerActionContext& Context)
{
	int32 BestIndex = 0;
	float BestWeight = 0.f;
	FPredictionResult NewPrediction;
	if (!TableOfAttacks.Contains(Context.BossAction))
	{
		NewPrediction = FPredictionResult(EPlayerAction::None, 0.f);
		return NewPrediction;
	}
	auto& ActionHistory = TableOfAttacks[Context.BossAction];
	auto& AllWeights = ActionHistory.Weights;
	for (int32 Index = 0; Index < static_cast<int32>(EPlayerAction::Count); Index++)
	{
		auto& CurrentWeight = AllWeights[Index];
		if (CurrentWeight > BestWeight)
		{
			BestWeight = CurrentWeight;
			BestIndex = Index;
		}
	}

	const float SampleScale = FMath::Clamp(
		ActionHistory.Total / FMath::Max(static_cast<float>(MinSamplesForConfidence), 1.f), 0.f, 1.f);

	NewPrediction = FPredictionResult(static_cast<EPlayerAction>(BestIndex),
	                                  (BestWeight / ActionHistory.Total) * SampleScale);
	return NewPrediction;
}

void UPlayerAdaptation::UpdateCurrentPrediction(const FPredictionResult& Prediction)
{
	CurrentPrediction = Prediction;
	CurrentConfidence = Prediction.Confidence;
}


// Called every frame
void UPlayerAdaptation::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsWindowOpen)
	{
		return;
	}

	WindowTime -= DeltaTime;
	if (WindowTime <= 0.f)
	{
		CloseTrackingWindow();
	}
}
