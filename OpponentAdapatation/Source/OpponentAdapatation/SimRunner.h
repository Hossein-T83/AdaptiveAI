// Drives the adaptation on/off comparison: runs N fights across four conditions, resetting
// between each, and writes the results out at the end.
//
// Put this on your GameMode. Fights take real time, so this cannot be a for-loop — each fight
// chains into the next from OnFightEnded.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimRunner.generated.h"

class UScriptedPlayer;
class UFightRecorder;
class UAIDirector;
class UPlayerAdaptation;

USTRUCT()
struct FSimCondition
{
	GENERATED_BODY()

	bool bAdaptationOn = false;
	bool bRandomPolicy = false;

	FString Name() const
	{
		return FString::Printf(TEXT("%s"), bRandomPolicy ? TEXT("Random") : TEXT("Habitual"));
	}
};

UCLASS()
class OPPONENTADAPATATION_API ASimRunner : public AActor
{
	GENERATED_BODY()

public:
	ASimRunner();

	/** Starts the whole run. Console: boss.RunSim 10 */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void RunSimulation(int32 InFightsPerCondition = 10);

	/** Call from wherever the fight ends — player death or boss death. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void OnFightEnded(bool bPlayerWon);

	/** Stops early and writes out whatever has been collected. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void AbortSimulation();

	UFUNCTION(BlueprintPure, Category = "Sim")
	bool IsRunning() const { return bRunning; }

	/** Time scale applied while the simulation runs. 10 makes a 30s fight take 3s. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	float SimTimeDilation = 5.f;

	/** Safety net: abandon a fight that runs longer than this (seconds, real time). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	float FightTimeout = 120.f;

protected:
	virtual void BeginPlay() override;

private:
	void BeginNextFight();
	void FinishSimulation();
	void OnFightTimeout();

	/** Resolved in BeginPlay. Replace the finder calls with your own actor classes. */
	UPROPERTY() TObjectPtr<UScriptedPlayer> Bot;
	UPROPERTY() TObjectPtr<UFightRecorder> Recorder;
	UPROPERTY() TObjectPtr<UAIDirector> Director;
	UPROPERTY() TObjectPtr<UPlayerAdaptation> Model;
	UPROPERTY() TObjectPtr<AActor> Enemy;
	UPROPERTY() TObjectPtr<AActor> Player;

	TArray<FSimCondition> Conditions;

	bool bRunning = false;
	int32 FightsPerCondition = 10;
	int32 ConditionIndex = 0;
	int32 FightInCondition = 0;
	int32 GlobalFightIndex = 0;

	FTimerHandle TimeoutTimer;
};