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

