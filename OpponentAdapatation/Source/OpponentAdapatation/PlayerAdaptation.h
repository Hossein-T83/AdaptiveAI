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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adaptation|Tuning", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float DecayPerNewContext = 0.9f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	void CloseTrackingWindow();

	bool bIsWindowOpen = false;
	FPlayerActionContext CurrentContext;
	float WindowTime = 0.f;
	TMap<uint32, FActionCounts> TableOfContext;

		
};
