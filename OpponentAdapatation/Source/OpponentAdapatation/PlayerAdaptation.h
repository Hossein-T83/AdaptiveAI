// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdaptationUtility.h"
#include "Components/ActorComponent.h"
#include "PlayerAdaptation.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPPONENTADAPATATION_API UPlayerAdaptation : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerAdaptation();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void OpenTrackingWindow(const FPlayerActionContext& Context, float WindowSeconds = 1.5f);

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void RecordPlayerAction(EPlayerAction Action);

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void GradePrediction(EPlayerAction Action);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptation|Tuning",
		meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float DecayPerNewContext = 0.9f;

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline FPredictionResult GetCurrentPrediction() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline FString GetContextAsString() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline float GetAccuracy() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation|Tuning",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ConfidenceThreshold = 0.7f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation|Tuning")
	int32 MinSamplesForConfidence = 5.f;

	FPredictionResult PredictBasedOnContext(const FPlayerActionContext& Context);
	FPredictionResult PredictBasedOnAttack(const FPlayerActionContext& Context);

	void UpdateCurrentPrediction(const FPredictionResult& Prediction);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogTable() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogCurrentContext() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogPrediction() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogAccuracy() const;

private:
	void CloseTrackingWindow();

	bool bIsWindowOpen = false;
	FPlayerActionContext CurrentContext;
	float WindowTime = 0.f;
	TMap<FGameplayTagContainer, FActionCounts> TableOfContext{};
	TMap<int32, FActionCounts> TableOfAttacks{};
	float CurrentConfidence = 0.f;
	FPredictionResult CurrentPrediction;
	float TotalCorrectPredictions = 0.f;
	float TotalPredictions = 0.f;
	float Accuracy = 0.f;

public:
	void HardReset();
	void SoftReset();
};
