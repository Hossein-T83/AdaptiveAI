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
	if (bIsWindowOpen)
	{
		CloseTrackingWindow();
	}

	bIsWindowOpen = true;
	CurrentContext = Context;
	WindowTime = WindowSeconds;

	Predict(Context.ToKey());
}

void UPlayerAdaptation::RecordPlayerAction(EPlayerAction Action)
{
	if (!bIsWindowOpen /*|| Action == EPlayerAction::None*/)
	{
		return;
	}

	bIsWindowOpen = false;
	const uint32 ContextIndex = (uint32)CurrentContext.ToKey();
	FActionCounts& ActionRef = TableOfContext.FindOrAdd(ContextIndex);

	for (float& Weight : ActionRef.Weights)
	{
		Weight *= DecayPerNewContext;
	}
    ActionRef.Total *= DecayPerNewContext;

	ActionRef.Weights[(int32)Action] += 1.f;
	ActionRef.Total += 1.f;
}

void UPlayerAdaptation::LogTable() const
{
	UE_LOG(LogTemp, Log, TEXT("Player Adaptation Table:"));
	for (const auto& Pair : TableOfContext)
	{
		const uint32 ContextIndex = Pair.Key;
		const FActionCounts& Counts = Pair.Value;
		FPlayerActionContext Context = FPlayerActionContext::FromKey(ContextIndex);
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

void UPlayerAdaptation::LogPrediction() const
{
	UE_LOG(LogTemp, Log, TEXT("Current Prediction: Action: %s, Confidence: %.2f"), *UEnum::GetValueAsString(CurrentPrediction.Action), CurrentPrediction.Confidence);
}

void UPlayerAdaptation::LogAccuracy() const
{
	UE_LOG(LogTemp, Log, TEXT("Prediction Accuracy: %.2f%% (%d/%d)"), Accuracy * 100.f, static_cast<int32>(TotalCorrectPredictions), static_cast<int32>(TotalPredictions));
}

void UPlayerAdaptation::GradePrediction(EPlayerAction Action)
{
	if (CurrentPrediction.Confidence > ConfidenceThreshold)
	{
		if (Action == CurrentPrediction.Action)
		{
			TotalCorrectPredictions += 1.f;
			UE_LOG(LogTemp, Log, TEXT("Prediction correct! Action: %s, Confidence: %.2f"), *UEnum::GetValueAsString(Action), CurrentPrediction.Confidence);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Prediction incorrect. Predicted: %s, Actual: %s, Confidence: %.2f"), *UEnum::GetValueAsString(CurrentPrediction.Action), *UEnum::GetValueAsString(Action), CurrentPrediction.Confidence);
		}
	}

	Accuracy = TotalCorrectPredictions / TotalPredictions;
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
	RecordPlayerAction(EPlayerAction::None);
	GradePrediction(EPlayerAction::None);
	bIsWindowOpen = false;
	
	LogTable();
	LogPrediction();
	LogAccuracy();
}

void UPlayerAdaptation::Predict(uint32 ContextKey)
{
	int32 BestIndex = 0;
	float BestWeight = 0.f;
	if (!TableOfContext.Contains(ContextKey))
	{
		CurrentConfidence = 0.f;
		CurrentPrediction = FPredictionResult(EPlayerAction::None, CurrentConfidence);
		return;
	}
	auto& ActionHistory = TableOfContext[ContextKey];
	auto& AllWeights = ActionHistory.Weights;
	for (int32 Index = 0; Index < int32(EPlayerAction::Count); Index++)
	{
		auto& CurrentWeight = AllWeights[Index];
		if (CurrentWeight > BestWeight)
		{
			BestWeight = CurrentWeight;
			BestIndex = Index;
		}
	}

	CurrentConfidence = BestWeight / ActionHistory.Total;
	CurrentPrediction = FPredictionResult(EPlayerAction(BestIndex), CurrentConfidence);

	if (CurrentPrediction.Confidence > ConfidenceThreshold)
	{
		TotalPredictions += 1.f;
	}
}


// Called every frame
void UPlayerAdaptation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

