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
	if (!bIsWindowOpen || Action == EPlayerAction::None)
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
			UE_LOG(LogTemp, Log, TEXT("  Action: %s, Weight: %.2f, Confidence: %.2f"), *UEnum::GetValueAsString(Action), Weight, Confidence);
		}
	}
	UE_LOG(LogTemp, Log, TEXT(""));

}

void UPlayerAdaptation::GradePrediction(EPlayerAction Action)
{
	if (!bIsWindowOpen)
	{
		return;
	}
	if (CurrentPrediction.Confidence > ConfidenceThreshold)
	{
		if (Action == CurrentPrediction.Action)
		{
			TotalCorrectPredictions += 1.f;
			Accuracy = TotalCorrectPredictions / TotalPredictions; // Assuming at least one prediction has been made
			UE_LOG(LogTemp, Log, TEXT("Prediction correct! Action: %s, Confidence: %.2f"), *UEnum::GetValueAsString(Action), CurrentPrediction.Confidence);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Prediction incorrect. Predicted: %s, Actual: %s, Confidence: %.2f"), *UEnum::GetValueAsString(CurrentPrediction.Action), *UEnum::GetValueAsString(Action), CurrentPrediction.Confidence);
		}
	}
	
	
}

// Called when the game starts
void UPlayerAdaptation::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UPlayerAdaptation::CloseTrackingWindow()
{
	bIsWindowOpen = false;
}

void UPlayerAdaptation::Predict(uint32 ContextKey)
{
	int32 BestIndex = 0;
	float BestWeight = 0.f;
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

