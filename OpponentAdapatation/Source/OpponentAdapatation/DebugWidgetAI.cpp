// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidgetAI.h"
#include "AdaptationUtility.h"
#include "Components/TextBlock.h"


void UDebugWidgetAI::Refresh(const FPredictionResult& Prediction, const FString& Attack, const FString& Scores,
                             const EPlayerAction PlayerAction, const float DeltaTime)
{
	if (PredictionText)
	{
		PredictionText->SetText(FText::FromString(
			FString::Printf(TEXT("Predicted action: %s"), *UEnum::GetValueAsString(Prediction.Action))));
		Prediction.Action == PlayerAction
			? PredictionText->SetColorAndOpacity(FColor::Green) : PredictionText->SetColorAndOpacity(FColor::Red);
	}
	if (TimerText)
	{
		if (TimerRemaining > 0)
		{
			TimerRemaining -= DeltaTime;
		}
		TimerText->SetText(FText::AsNumber(TimerRemaining));
	}
	if (SelectedAttack)
	{
		SelectedAttack->SetText(FText::FromString(
			FString::Printf(TEXT("Selected attack: %s"), *Attack)));
	}
	if (Confidence)
	{
		Confidence->SetText(FText::FromString(
			FString::Printf(TEXT("Confidence: %.2f"), Prediction.Confidence)));
	}

	if (ScoresText)
	{
		ScoresText->SetText(FText::FromString(
			FString::Printf(TEXT("Attack scores: %s"), *Scores)));
	}
}

void UDebugWidgetAI::StartTimer(float Time)
{
	TimerRemaining = Time;
}
