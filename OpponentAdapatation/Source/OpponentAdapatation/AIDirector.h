// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDirector.generated.h"


class UFightRecorder;
class UScriptedPlayer;
struct FGameplayTagContainer;
struct FBossAttack;
class URuleSet;
class UPlayerAdaptation;
struct FFightContext;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPPONENTADAPATATION_API UAIDirector : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIDirector();
	void ScoreAttacks(const FGameplayTagContainer& Context, int32& HighestScoreId, float& HighestScore);

	/**
	 * Based on the given context, chooses the best available attack.
	 * @param Context Gameplaytag container of current fight context.
	 * @return The attack with the highest score overall.
	 */
	UFUNCTION(BlueprintCallable, Category = "Director")
	const FBossAttack ChooseNextAttack(const FGameplayTagContainer& Context);
	/**
	 * Calculates delay between each attack based on the given context and the ruleset.
	 * @param Context Gameplaytag container of current fight context.
	 * @return Final delay
	 */
	UFUNCTION(BlueprintCallable, Category = "Director")
	float CalculateFinalDelay(const FGameplayTagContainer& Context) const;
	/**
	 * Converting Attack ID to its index in active attacks array.
	 * @param Id Attack ID
	 * @return Index of given attack in array of active attacks.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 IdToIndex(int32 Id) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UPlayerAdaptation> Model;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Instanced, Category = "Rules")
	TObjectPtr<URuleSet> Rule;

	UPROPERTY(EditAnywhere, Category = "Rules")
	float BaseDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FBossAttack> ValidAttacks;

	TArray<FBossAttack> AttacksOnCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
	float RecordingWindowTime = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Flags")
	bool AdaptationEnabled = true;

	// True if you don't want attack specific data be taken into consideration.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
	bool FightContextOnly = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonus")
	float ContextBonus = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonus")
	float AttackBonus = 10.f;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Result")
	TMap<FName, float> Scores;

	UFUNCTION(BlueprintCallable, Category = "Result")
	FString GetScoresString() const;
	
	const TObjectPtr<UFightRecorder>& GetFightRecorder() const
	{
		return Recorder;
	}
	const TObjectPtr<UScriptedPlayer>& GetScripter() const
	{
		return Bot;
	}

private:
	TObjectPtr<UScriptedPlayer> Bot;
	TObjectPtr<UFightRecorder> Recorder;
};
