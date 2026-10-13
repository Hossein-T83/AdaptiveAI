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

public:
	/**
	 * Starts the data collection process 
	 * @param Context Current context of fight
	 * @param WindowSeconds How long the action tracking window should be open
	 */
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void OpenTrackingWindow(const FPlayerActionContext& Context, float WindowSeconds = 1.5f);

	/**
	 * Getting the latest response of the player, grading the prediction based on the recorded action and decays the older values
	 * @param Action Player action response 
	 */
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void RecordPlayerAction(EPlayerAction Action);

	/**
	 * Grade prediction based on the player's action
	 * @param Action Player's action
	 */
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void GradePrediction(EPlayerAction Action);

	/**
	 * Returns the latest prediction
	 * @return Current prediction of the model
	 */
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline FPredictionResult GetCurrentPrediction() const;

	/**
	 * Returns the current context as string
	 * @return Current fight context as string
	 */
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline FString GetContextAsString() const;

	/**
	 * Returns the current accuracy of the model
	 * @return Accuracy of the model
	 */
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline float GetAccuracy() const;

public:
	/** Old habits fade at this rate. Lower = forgets faster = player escapes the read sooner.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptation|Tuning",
		meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float DecayPerNewContext = 0.9f;

	/* Below this, the boss ignores the model entirely. Prevents acting on noise. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation|Tuning",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ConfidenceThreshold = 0.7f;

	/*If the total samples collected is lower than this value, confidence goes down significantly to prevent acting on low amount of samples.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation|Tuning")
	int32 MinSamplesForConfidence = 5.f;

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
	/**
	 * Resets everything. For when you don't want data from previous fight. 
	 */
	void HardReset();
	/**
	 * Only reset fight specific variables. For when you want the model to remember previous fight's data.
	 */
	void SoftReset();

	FPredictionResult PredictBasedOnContext(const FPlayerActionContext& Context);
	FPredictionResult PredictBasedOnAttack(const FPlayerActionContext& Context);

	void UpdateCurrentPrediction(const FPredictionResult& Prediction);
};
