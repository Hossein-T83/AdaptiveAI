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

	// Packs the context into a single map key
	uint32 ToKey() const
	{
		return static_cast<uint32>(Distance)
			| (static_cast<uint32>(HealthBucket) << 8)
			| (static_cast<uint32>(BossAction) << 16);
	}

	// Unpacks a key into a context struct
	static FPlayerActionContext FromKey(uint32 Key)
	{
		FPlayerActionContext Context;
		Context.Distance = static_cast<EDistance>(Key & 0xFF);
		Context.HealthBucket = static_cast<uint8>((Key >> 8) & 0xFF);
		Context.BossAction = static_cast<uint8>((Key >> 16) & 0xFF);
		return Context;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Distance=%d, HealthBucket=%d, BossAction=%d"),
			static_cast<uint8>(Distance), HealthBucket, BossAction);
	}
};

USTRUCT(BlueprintType)
struct FPredictionResult
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "Adaptation")
	EPlayerAction Action = EPlayerAction::None;
	UPROPERTY(BlueprintReadOnly, Category = "Adaptation")
	float Confidence = 0.f;
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

USTRUCT(BlueprintType)
struct FActionCounts
{
	GENERATED_BODY()

	UPROPERTY()
	float Weights[static_cast<int32>(EPlayerAction::Count)] = {};

	UPROPERTY()
	float Total = 0.f;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPPONENTADAPATATION_API UPlayerAdaptation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerAdaptation();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void OpenTrackingWindow(const FPlayerActionContext& Context, float WindowSeconds = 1.5f);

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void RecordPlayerAction(EPlayerAction Action);

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogTable() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogPrediction() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogAccuracy() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	void GradePrediction(EPlayerAction Action);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptation|Tuning", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float DecayPerNewContext = 0.9f;
	
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline FPredictionResult GetCurrentPrediction() const;
	
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline FString GetContextAsString() const;
	
	UFUNCTION(BlueprintCallable, Category = "Adaptation")
	inline float GetAccuracy() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	void CloseTrackingWindow();
	void Predict(uint32 ContextKey);

	bool bIsWindowOpen = false;
	FPlayerActionContext CurrentContext;
	float WindowTime = 0.f;
	TMap<uint32, FActionCounts> TableOfContext{};
	float CurrentConfidence = 0.f;
	FPredictionResult CurrentPrediction;
	float ConfidenceThreshold = 0.7f;
	float TotalCorrectPredictions = 0.f;
	float TotalPredictions = 0.f;
	float Accuracy = 0.f;

		
};
