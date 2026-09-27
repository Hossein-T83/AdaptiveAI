// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidgetAI.h"

#include <string>
#include "Components/TextBlock.h"


void UDebugWidgetAI::Refresh(const FString& State, const FString& Prediction, float DeltaTime)
{
	if (StateText) StateText->SetText(FText::FromString(Prediction));
	if (PredictionText)
	{
		if (TimerRemaining > 0)
		{
			TimerRemaining -= DeltaTime;
		}
		PredictionText->SetText(FText::AsNumber(TimerRemaining));
	}
}

void UDebugWidgetAI::StartTimer(float Time)
{
	TimerRemaining = Time;
}
