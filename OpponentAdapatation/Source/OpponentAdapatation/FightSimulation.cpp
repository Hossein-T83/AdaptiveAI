#include "FightSimulation.h"

#include "OpponentAdapatationCharacter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

/* ------------------------------------------------------------------ */
/*  Scripted player                                                     */
/* ------------------------------------------------------------------ */

UScriptedPlayer::UScriptedPlayer()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UScriptedPlayer::BeginPlay()
{
	Super::BeginPlay();

	Rng.Initialize(RandomSeed != 0 ? RandomSeed : FDateTime::Now().GetTicks());

	// Default pool: every action except None and the Count sentinel.
	if (AllowedActions.Num() == 0)
	{
		for (int32 i = 1; i < static_cast<int32>(EPlayerAction::Count); ++i)
		{
			AllowedActions.Add(static_cast<EPlayerAction>(i));
		}
	}
}

void UScriptedPlayer::OnBossAttackStarted(int32 BossAttackId)
{
	if (!bEnabled || !GetWorld())
	{
		return;
	}

	// Jittered delay: a perfectly periodic response would be a signal in itself, and the
	// observation window pairing should not depend on frame-exact timing.
	const float Delay = FMath::Max(0.01f,
	                               ReactionDelay + Rng.FRandRange(-ReactionJitter, ReactionJitter));

	GetWorld()->GetTimerManager().SetTimer(
		ReactionTimer, this, &UScriptedPlayer::PerformQueuedAction, Delay, false);
}

void UScriptedPlayer::Abort()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReactionTimer);
	}
}

EPlayerAction UScriptedPlayer::ChooseAction()
{
	if (Policy == EBotPolicy::Habitual && Rng.FRand() <= HabitConsistency)
	{
		bLastWasHabit = true;
		return HabitAction;
	}

	bLastWasHabit = false;

	if (AllowedActions.Num() == 0)
	{
		return EPlayerAction::None;
	}
	return AllowedActions[Rng.RandRange(0, AllowedActions.Num() - 1)];
}

void UScriptedPlayer::PerformQueuedAction()
{
	// in PerformQueuedAction, after choosing
	UE_LOG(LogTemp, Warning, TEXT("SIM: bot acts %s"), *UEnum::GetValueAsString(LastAction));
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	LastAction = ChooseAction();
	if (LastAction == EPlayerAction::None)
	{
		return;
	}

	// INTEGRATION POINT
	// -----------------
	// The bot must perform the action through the SAME path the player's input uses, so the
	// roll actually happens, i-frames apply, and the model records it exactly as it would a
	// human response. Add a function on the character that both the input handlers and this
	// call into:
	//
	//     void AOpponentAdapatationCharacter::PerformAction(EPlayerAction Action);
	//
	// and have Roll() / Dodge() / Block() call it rather than duplicating their bodies here.
	//
	// Replace the cast below with your character class.

	if (AOpponentAdapatationCharacter* Character = Cast<AOpponentAdapatationCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AOpponentAdapatationCharacter::StaticClass())))
	{
		Character->PerformAction(LastAction);
	}
}

/* ------------------------------------------------------------------ */
/*  Fight recorder                                                      */
/* ------------------------------------------------------------------ */

UFightRecorder::UFightRecorder()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFightRecorder::StartFight(int32 FightIndex, bool bAdaptationOn, const FString& PolicyName)
{
	CloseOpenBout();

	CurrentFightIndex = FightIndex;
	bCurrentAdaptationOn = bAdaptationOn;
	CurrentPolicy = PolicyName;
	FightStartTime = FPlatformTime::Seconds();
}

void UFightRecorder::RecordBossAttack(int32 AttackId, const FString& PredictedAction, float Confidence)
{
	// A new attack closes the previous bout — its duration is the opening the player had.

	// in the recorder's RecordBossAttack
	UE_LOG(LogTemp, Warning, TEXT("SIM: boss attack %d"), AttackId);


	CloseOpenBout();

	OpenBout = FBoutRecord();
	OpenBout.FightIndex = CurrentFightIndex;
	OpenBout.bAdaptationOn = bCurrentAdaptationOn;
	OpenBout.Policy = CurrentPolicy;
	OpenBout.BossAttackId = AttackId;
	OpenBout.PredictedAction = PredictedAction;
	OpenBout.PredictionConfidence = Confidence;
	OpenBout.PlayerAction = TEXT("None");

	BoutStartTime = FPlatformTime::Seconds();
	bBoutOpen = true;
}

void UFightRecorder::RecordPlayerAction(EPlayerAction Action, bool bWasHabit)
{
	if (!bBoutOpen)
	{
		return;
	}

	OpenBout.PlayerAction = UEnum::GetValueAsString(Action);
	OpenBout.bWasHabit = bWasHabit;
}

void UFightRecorder::RecordPlayerDamaged(float Amount)
{
	if (!bBoutOpen)
	{
		return;
	}

	OpenBout.bPlayerHit = true;
	OpenBout.DamageTaken += Amount;
}

void UFightRecorder::CloseOpenBout()
{
	if (!bBoutOpen)
	{
		return;
	}

	OpenBout.BoutDuration = static_cast<float>(FPlatformTime::Seconds() - BoutStartTime);
	Bouts.Add(OpenBout);
	bBoutOpen = false;
}

void UFightRecorder::EndFight(bool bPlayerWon)
{
	CloseOpenBout();

	const double Duration = FPlatformTime::Seconds() - FightStartTime;
	UE_LOG(LogTemp, Log, TEXT("Fight %d finished: adaptation=%s policy=%s won=%s duration=%.1fs"),
	       CurrentFightIndex,
	       bCurrentAdaptationOn ? TEXT("ON") : TEXT("OFF"),
	       *CurrentPolicy,
	       bPlayerWon ? TEXT("yes") : TEXT("no"),
	       Duration);
}

bool UFightRecorder::FlushToCsv()
{
	if (Bouts.Num() == 0)
	{
		return true;
	}

	const FString Dir = FPaths::ProjectSavedDir() / TEXT("FightSim");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*Dir))
	{
		PlatformFile.CreateDirectoryTree(*Dir);
	}

	const FString Path = Dir / FileName;
	const bool bNewFile = !PlatformFile.FileExists(*Path);

	FString Out;
	if (bNewFile)
	{
		Out += TEXT("FightIndex,AdaptationOn,Policy,BossAttackId,PredictedAction,")
			TEXT("Confidence,PlayerAction,WasHabit,PlayerHit,DamageTaken,BoutDuration\n");
	}

	for (const FBoutRecord& R : Bouts)
	{
		Out += FString::Printf(TEXT("%d,%d,%s,%d,%s,%.3f,%s,%d,%d,%.1f,%.3f\n"),
		                       R.FightIndex,
		                       R.bAdaptationOn ? 1 : 0,
		                       *R.Policy,
		                       R.BossAttackId,
		                       *R.PredictedAction,
		                       R.PredictionConfidence,
		                       *R.PlayerAction,
		                       R.bWasHabit ? 1 : 0,
		                       R.bPlayerHit ? 1 : 0,
		                       R.DamageTaken,
		                       R.BoutDuration);
	}

	const bool bOk = FFileHelper::SaveStringToFile(
		Out, *Path, FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(),
		bNewFile ? FILEWRITE_None : FILEWRITE_Append);

	if (bOk)
	{
		UE_LOG(LogTemp, Log, TEXT("FightSim: wrote %d bouts to %s"), Bouts.Num(), *Path);
		Bouts.Reset();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FightSim: failed to write %s"), *Path);
	}

	return bOk;
}

void UFightRecorder::LogSummary() const
{
	// Four conditions: adaptation on/off crossed with habitual/random policy.
	struct FAgg
	{
		int32 HabitUsed = 0;
		int32 HabitPunished = 0;
		int32 TotalBouts = 0;
		float TotalDamage = 0.f;
		float TotalOpening = 0.f;
	};

	TMap<FString, FAgg> Agg;

	for (const FBoutRecord& R : Bouts)
	{
		const FString Key = FString::Printf(TEXT("%s / adaptation %s"),
		                                    *R.Policy, R.bAdaptationOn ? TEXT("ON") : TEXT("OFF"));

		FAgg& A = Agg.FindOrAdd(Key);
		A.TotalBouts++;
		A.TotalDamage += R.DamageTaken;
		A.TotalOpening += R.BoutDuration;

		if (R.bWasHabit)
		{
			A.HabitUsed++;
			if (R.bPlayerHit)
			{
				A.HabitPunished++;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("=== Fight simulation summary ==="));
	for (const TPair<FString, FAgg>& Pair : Agg)
	{
		const FAgg& A = Pair.Value;
		const float HabitSuccess = A.HabitUsed > 0
			                           ? 1.f - (static_cast<float>(A.HabitPunished) / A.HabitUsed)
			                           : 0.f;

		UE_LOG(LogTemp, Log,
		       TEXT("%s | bouts %d | habit success %.0f%% (%d/%d) | avg opening %.2fs | dmg/bout %.1f"),
		       *Pair.Key,
		       A.TotalBouts,
		       HabitSuccess * 100.f,
		       A.HabitUsed - A.HabitPunished, A.HabitUsed,
		       A.TotalBouts > 0 ? A.TotalOpening / A.TotalBouts : 0.f,
		       A.TotalBouts > 0 ? A.TotalDamage / A.TotalBouts : 0.f);
	}
}
