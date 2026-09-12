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
	MoveLeft,
	MoveRight,
	MoveBack,
	MoveForward,
	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EBossAttacks : uint8
{
	Attack1,
	Attack2, 
	Attack3, 
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
struct FFightContext
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	EDistance Distance = EDistance::Medium;
	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	uint8 HealthBucket = 3;
	
	FString ToString() const
	{
		return FString::Printf(TEXT("Distance=%d, HealthBucket=%d"),
			static_cast<uint8>(Distance), HealthBucket);
	}
	
	bool operator==(const FFightContext& Other) const
	{
		return Distance == Other.Distance
			&& HealthBucket == Other.HealthBucket;
	}

	friend uint32 GetTypeHash(const FFightContext& C)
	{
		return HashCombine(GetTypeHash(C.Distance), GetTypeHash(C.HealthBucket));
	}
};

USTRUCT(BlueprintType)
struct FPlayerActionContext
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	FFightContext Combat;
	
	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	uint8 BossAction = 0;

	FString ToString() const
	{
		return FString::Printf(TEXT("%s, BossAction=%d"),*Combat.ToString(), BossAction);
	}
	
	bool operator==(const FPlayerActionContext& Other) const
	{
		return Combat == Other.Combat && BossAction == Other.BossAction;
	}

	friend uint32 GetTypeHash(const FPlayerActionContext& C)
	{
		return HashCombine(GetTypeHash(C.Combat), GetTypeHash(C.BossAction));
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
	void Predict(const FPlayerActionContext& Context);

	bool bIsWindowOpen = false;
	FPlayerActionContext CurrentContext;
	float WindowTime = 0.f;
	TMap<FPlayerActionContext, FActionCounts> TableOfContext{};
	float CurrentConfidence = 0.f;
	FPredictionResult CurrentPrediction;
	float ConfidenceThreshold = 0.7f;
	float TotalCorrectPredictions = 0.f;
	float TotalPredictions = 0.f;
	float Accuracy = 0.f;

		
};
