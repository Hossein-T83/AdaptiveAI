// Fight simulation harness: a scripted player with a fixed or random policy, and a recorder
// that writes per-bout results to CSV for the adaptation on/off comparison.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdaptationUtility.h"
#include "FightSimulation.generated.h"

/* ------------------------------------------------------------------ */
/*  Scripted player                                                     */
/* ------------------------------------------------------------------ */

UENUM(BlueprintType)
enum class EBotPolicy : uint8
{
	/** Always performs HabitAction. Perfectly predictable — the model should learn it fast. */
	Habitual,

	/** Uniform random over AllowedActions. The control: adaptation should NOT hurt this bot. */
	Random
};

UCLASS(ClassGroup = (Sim), meta = (BlueprintSpawnableComponent))
class OPPONENTADAPATATION_API UScriptedPlayer : public UActorComponent
{
	GENERATED_BODY()

public:
	UScriptedPlayer();

	/** Call this the moment the boss commits to an attack — same point you open the tracking window. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void OnBossAttackStarted(int32 BossAttackId);

	/** Stops any queued response. Call on fight reset. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void Abort();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	bool bEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	EBotPolicy Policy = EBotPolicy::Habitual;

	/** The habit. Make sure at least one boss attack punishes it, or the test proves nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	EPlayerAction HabitAction = EPlayerAction::RollLeft;

	/** 1.0 = always the habit. Below 1.0 it falls back to a random allowed action, which is
	 *  useful for testing how much inconsistency the model tolerates. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HabitConsistency = 1.0f;

	/** Pool the Random policy draws from. Leave empty to use every action except None. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	TArray<EPlayerAction> AllowedActions;

	/** Delay before responding, so the bot does not react on the same frame the attack starts.
	 *  Randomised by +/- ReactionJitter to avoid a perfectly periodic signal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	float ReactionDelay = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	float ReactionJitter = 0.02f;

	/** Fixed seed so a run can be repeated exactly. Set to 0 for a time-based seed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	int32 RandomSeed = 1337;

	/** Last action the bot chose, and whether it was the habit. Read by the recorder. */
	UFUNCTION(BlueprintPure, Category = "Sim")
	EPlayerAction GetLastAction() const { return LastAction; }

	UFUNCTION(BlueprintPure, Category = "Sim")
	bool WasLastActionTheHabit() const { return bLastWasHabit; }

protected:
	virtual void BeginPlay() override;

private:
	void PerformQueuedAction();
	EPlayerAction ChooseAction();

	FTimerHandle ReactionTimer;
	FRandomStream Rng;

	EPlayerAction LastAction = EPlayerAction::None;
	bool bLastWasHabit = false;
};

/* ------------------------------------------------------------------ */
/*  Fight recorder                                                      */
/* ------------------------------------------------------------------ */

/** One boss attack and the player's response to it. */
USTRUCT()
struct FBoutRecord
{
	GENERATED_BODY()

	int32 FightIndex = 0;
	bool bAdaptationOn = false;
	FString Policy;

	int32 BossAttackId = 0;
	FString PredictedAction;
	float PredictionConfidence = 0.f;

	FString PlayerAction;
	bool bWasHabit = false;

	/** True if the player took damage during this bout — i.e. the response failed. */
	bool bPlayerHit = false;
	float DamageTaken = 0.f;

	/** Seconds from the attack committing to the next attack committing. The player's opening. */
	float BoutDuration = 0.f;
};

UCLASS(ClassGroup = (Sim), meta = (BlueprintSpawnableComponent))
class OPPONENTADAPATATION_API UFightRecorder : public UActorComponent
{
	GENERATED_BODY()

public:
	UFightRecorder();

	UFUNCTION(BlueprintCallable, Category = "Sim")
	void StartFight(int32 FightIndex, bool bAdaptationOn, const FString& PolicyName);

	/** Call when the boss commits to an attack. Closes the previous bout and opens a new one. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void RecordBossAttack(int32 AttackId, const FString& PredictedAction, float Confidence);

	/** Call when the player acts. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void RecordPlayerAction(EPlayerAction Action, bool bWasHabit);

	/** Call when the player takes damage. Marks the open bout as a failed response. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void RecordPlayerDamaged(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Sim")
	void EndFight(bool bPlayerWon);

	/** Appends everything buffered to Saved/FightSim/<FileName>. Safe to call repeatedly. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	bool FlushToCsv();

	/** Prints a per-condition summary to the log — the numbers for the slide. */
	UFUNCTION(BlueprintCallable, Category = "Sim")
	void LogSummary() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sim")
	FString FileName = TEXT("fightsim.csv");

private:
	void CloseOpenBout();

	TArray<FBoutRecord> Bouts;

	FBoutRecord OpenBout;
	bool bBoutOpen = false;
	double BoutStartTime = 0.0;

	int32 CurrentFightIndex = 0;
	bool bCurrentAdaptationOn = false;
	FString CurrentPolicy;
	double FightStartTime = 0.0;
};