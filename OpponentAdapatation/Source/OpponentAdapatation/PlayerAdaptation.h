// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDistance : uint8
{
	Close,
	Medium,
	Far,
	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EHealthBucket : uint8
{
	Low,
	Medium,
	High,
	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FBossAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	int32 Id = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FName AttackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Cooldown = 2.f;
	
	float Timer = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score", meta = (Categories = "Score"))
	TMap<FGameplayTag, float> Weights;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TArray<EPlayerAction> PunishableActions;
	
	bool operator==(const FBossAttack& Other) const
	{
		return Id == Other.Id;
	}
};


USTRUCT(BlueprintType)
struct FFightContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	FGameplayTagContainer Tags;
	

	bool operator==(const FFightContext& Other) const
	{
		return Tags == Other.Tags;
	}

};

USTRUCT(BlueprintType)
struct FPlayerActionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	FFightContext Combat;

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	int32 BossAction = 0;

	FString ToString() const
	{
		return FString::Printf(TEXT("%s, BossAction=%d"), *Combat.Tags.ToString(), BossAction);
	}
	bool operator==(const FPlayerActionContext& Other) const
	{
		return Combat.Tags == Other.Combat.Tags && BossAction == Other.BossAction;
	}
	
	//Claude
	friend uint32 GetTypeHash(const FPlayerActionContext& C)
	{
		uint32 TagsHash = 0;
		for (const FGameplayTag& Tag : C.Combat.Tags)
		{
			// XOR is commutative, so insertion order doesn't affect the result
			TagsHash ^= GetTypeHash(Tag);
		}

		return HashCombine(TagsHash, GetTypeHash(C.BossAction));
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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation|Tuning", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ConfidenceThreshold = 0.7f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation|Tuning")
	int32 MinSamplesForConfidence = 5.f;
	
	FPredictionResult Predict(const FPlayerActionContext& Context);
	
	void UpdateCurrentPrediction(const FPredictionResult& Prediction);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogTable() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogPrediction() const;

	UFUNCTION(BlueprintCallable, Category = "Adaptation|Debug")
	void LogAccuracy() const;

private:
	void CloseTrackingWindow();

	bool bIsWindowOpen = false;
	FPlayerActionContext CurrentContext; 
	float WindowTime = 0.f;
	TMap<FPlayerActionContext, FActionCounts> TableOfContext{};
	float CurrentConfidence = 0.f;
	FPredictionResult CurrentPrediction;
	float TotalCorrectPredictions = 0.f;
	float TotalPredictions = 0.f;
	float Accuracy = 0.f;
};
