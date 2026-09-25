// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidgetAI.h"

#include <string>

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UDebugWidgetAI::NativeConstruct()
{
	Super::NativeConstruct();
	
	UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Root"));
	WidgetTree->RootWidget = Root;

	StateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StateText"));
	Root->AddChildToVerticalBox(StateText);

	PredictionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PredictionText"));
	Root->AddChildToVerticalBox(PredictionText);
}

void UDebugWidgetAI::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);
	PredictionText->SetText(FText::AsNumber(TimerRemaining));	
	if (TimerRemaining > 0)
	{
		TimerRemaining -= DeltaTime;
		PredictionText->SetText(FText::AsNumber(TimerRemaining));
	}
}

void UDebugWidgetAI::StartTimer(float Time)
{
	TimerRemaining = Time;
}
