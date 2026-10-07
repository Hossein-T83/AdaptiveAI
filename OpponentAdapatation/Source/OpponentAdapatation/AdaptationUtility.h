#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AdaptationUtility.generated.h"

UENUM(BlueprintType)
enum class EPlayerAction : uint8
{
	None,
	DodgeLeft,
	DodgeRight,
	DodgeBack,
	DodgeForward,
	Parry,
	Block,
	RollLeft,
	RollRight,
	RollBack,
	RollForward,
	Jump,
	Attack,
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
struct FPlayerActionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	FGameplayTagContainer Tags;

	UPROPERTY(BlueprintReadWrite, Category = "Adaptation")
	int32 BossAction = 0;

	FString ToString() const
	{
		return FString::Printf(TEXT("%s, BossAction=%d"), *Tags.ToString(), BossAction);
	}

	bool operator==(const FPlayerActionContext& Other) const
	{
		return BossAction == Other.BossAction
			&& Tags.Num() == Other.Tags.Num()
			&& Tags.HasAllExact(Other.Tags);
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
	
	bool operator==(const FPredictionResult& Other) const
	{
		return Action == Other.Action;
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

USTRUCT(BlueprintType)
struct FActionCounts
{
	GENERATED_BODY()

	UPROPERTY()
	float Weights[static_cast<int32>(EPlayerAction::Count)] = {};

	UPROPERTY()
	float Total = 0.f;
};

inline uint32 GetTypeHash(const FGameplayTagContainer& C)
{
	uint32 TagsHash = 0;
	for (const FGameplayTag& Tag : C)
	{
		TagsHash ^= GetTypeHash(Tag);
	}
	return TagsHash;
}