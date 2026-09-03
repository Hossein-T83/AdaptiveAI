// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAdaptation.generated.h"


UENUM(BlueprintType)
enum class EPlayerAction : uint8
{
	None,
	DodgeLeft,
	DodgeRight,
	DodgeBack,
	DodgeForward,
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDistance : uint8
{
	Close,
	Medium,
	Far,
	Count	UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FPlayerActionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	EDistance Distance = EDistance::Medium;

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	uint8 BossAction = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	uint8 HealthBucket = 3;

};

USTRUCT(BlueprintType)
struct FActionPrediction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Adaptation")
	EPlayerAction Action = EPlayerAction::None;

	UPROPERTY(BlueprintReadOnly, Category = "Adaptation")
	float Confidence = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Adaptation")
	float SampleWeight = 0.f;
};

USTRUCT()
struct FActionCounts
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Weights;

	UPROPERTY()
	float Total = 0.f;

	void EnsureSized();
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPPONENTADAPATATION_API UPlayerAdaptation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerAdaptation();

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void OpenTrackingWindow(const FPlayerActionContext& Context, float WindowSeconds = 1.5f);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	void CloseTrackingWindow();

	bool bIsWindowOpen = false;
	FPlayerActionContext CurrentContext;
	float WindowTime = 0.f;
	TArray<FActionCounts> ActionCounts;

		
};
