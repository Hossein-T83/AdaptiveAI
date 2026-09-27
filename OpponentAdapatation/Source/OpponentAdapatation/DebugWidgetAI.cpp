// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidgetAI.h"

#include <string>
#include "Components/TextBlock.h"


void UDebugWidgetAI::Refresh(const FString& State, const FString& Prediction, const FString& Attack, const float DeltaTime)
{
	if (PredictionText)
		PredictionText->SetText(FText::FromString(
			FString::Printf(TEXT("Predicted action: %s"), *Prediction)));
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
}

void UDebugWidgetAI::StartTimer(float Time)
{
	TimerRemaining = Time;
}
