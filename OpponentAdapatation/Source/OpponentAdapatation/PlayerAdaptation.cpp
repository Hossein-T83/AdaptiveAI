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

