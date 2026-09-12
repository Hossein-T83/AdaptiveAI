// Fill out your copyright notice in the Description page of Project Settings.


#include "AIDirector.h"

#include "PlayerAdaptation.h"

// Sets default values for this component's properties
UAIDirector::UAIDirector()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

EBossAttacks UAIDirector::ChooseNextAttack(const FFightContext& Context)
{
	uint8 RandAttack = FMath::RandRange(0, static_cast<int8>(EBossAttacks::Count));
	if (Model)
	{
		Model->OpenTrackingWindow(FPlayerActionContext{Context, RandAttack}, 1.5f);
	}
	return static_cast<EBossAttacks>(RandAttack);
}


// Called when the game starts
void UAIDirector::BeginPlay()
{
	Super::BeginPlay();
	Model = GetOwner()->FindComponentByClass<UPlayerAdaptation>();
}


// Called every frame
void UAIDirector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

