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
}

void UPlayerAdaptation::RecordPlayerAction(EPlayerAction Action)
{
	if (!bIsWindowOpen /*|| Action == EPlayerAction::None*/)
	{
		return;
	}

	GradePrediction(Action);

	bIsWindowOpen = false;
	FActionCounts& ActionRef = TableOfContext.FindOrAdd(CurrentContext);

	for (float& Weight : ActionRef.Weights)
	{
		Weight *= DecayPerNewContext;
	}
	ActionRef.Total *= DecayPerNewContext;

	ActionRef.Weights[static_cast<int32>(Action)] += 1.f;
	ActionRef.Total += 1.f;
}

void UPlayerAdaptation::LogTable() const
{
	UE_LOG(LogTemp, Log, TEXT("Player Adaptation Table:"));
	for (const auto& Pair : TableOfContext)
	{
		const FActionCounts& Counts = Pair.Value;
		FPlayerActionContext Context = Pair.Key;
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
	const auto& History = TableOfContext.Find(CurrentContext);
	UE_LOG(LogTemp, Log, TEXT("Context: %s"), *CurrentContext.ToString());
	for (int32 i = 0; i < static_cast<int32>(EPlayerAction::Count); ++i)
	{
		EPlayerAction Action = static_cast<EPlayerAction>(i);
		float Weight = History->Weights[i];
		float Confidence = 0;
		UE_LOG(LogTemp, Log, TEXT("  Action: %s, Weight: %.2f"), *UEnum::GetValueAsString(Action), Weight);
	}

	UE_LOG(LogTemp, Log, TEXT(""));
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
	RecordPlayerAction(EPlayerAction::None);
	bIsWindowOpen = false;

	LogTable();
	LogPrediction();
	LogAccuracy();
}

FPredictionResult UPlayerAdaptation::Predict(const FPlayerActionContext& Context)
{
	int32 BestIndex = 0;
	float BestWeight = 0.f;
	FPredictionResult NewPrediction;
	if (!TableOfContext.Contains(Context))
	{
		NewPrediction = FPredictionResult(EPlayerAction::None, 0.f);
		return NewPrediction;
	}
	auto& ActionHistory = TableOfContext[Context];
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
