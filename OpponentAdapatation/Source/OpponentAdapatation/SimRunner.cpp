#include "SimRunner.h"

#include "FightSimulation.h"
#include "AIDirector.h"
#include "PlayerAdaptation.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

// Replace these with your own classes.
#include "CombatGameMode.h"
#include "IAutomationControllerManager.h"
#include "Variant_Combat/AI/CombatEnemy.h"
#include "OpponentAdapatationCharacter.h"
#include "OpponentAdapatationGameMode.h"
#include "Slate/SGameLayerManager.h"
#include "UObject/CookEnums.h"

ASimRunner::ASimRunner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASimRunner::BeginPlay()
{
	Super::BeginPlay();

	Enemy = UGameplayStatics::GetActorOfClass(GetWorld(), ACombatEnemy::StaticClass());
	Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Enemy)
	{
		Director = Enemy->FindComponentByClass<UAIDirector>();
		Model = Enemy->FindComponentByClass<UPlayerAdaptation>();
	}
	if (Player)
	{
		Bot = Director->GetScripter();
	}

	// The recorder can live on either; check both so placement is flexible.
	Recorder = FindComponentByClass<UFightRecorder>();
	if (!Recorder && Enemy)
	{
		Recorder = Director->GetFightRecorder();
	}

	// Four conditions: the two policies crossed with adaptation on and off. The random policy
	// is the control — if adaptation also hurts the random bot, the boss simply got stronger
	// and the result says nothing about adaptation.
	Conditions = {
		{ false, false },   // habitual, adaptation off  — the baseline
		{ true,  false },   // habitual, adaptation on   — should do measurably worse
		{ false, true  },   // random,   adaptation off
		{ true,  true  },   // random,   adaptation on   — should be roughly unchanged
	};
}

void ASimRunner::RunSimulation(int32 InFightsPerCondition)
{
	if (bRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("SimRunner: already running"));
		return;
	}

	if (!Bot || !Recorder || !Director || !Model)
	{
		UE_LOG(LogTemp, Error,
			TEXT("SimRunner: missing components — bot=%d recorder=%d director=%d model=%d"),
			Bot != nullptr, Recorder != nullptr, Director != nullptr, Model != nullptr);
		return;
	}

	FightsPerCondition = FMath::Max(1, InFightsPerCondition);
	ConditionIndex = 0;
	FightInCondition = 0;
	GlobalFightIndex = 0;
	bRunning = true;

	Bot->bEnabled = true;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SimTimeDilation);

	UE_LOG(LogTemp, Log, TEXT("SimRunner: starting %d fights x %d conditions"),
		FightsPerCondition, Conditions.Num());

	BeginNextFight();
}

void ASimRunner::BeginNextFight()
{
	// top of BeginNextFight
	UE_LOG(LogTemp, Warning, TEXT("SIM: begin fight %d"), GlobalFightIndex);

	
	if (!bRunning)
	{
		return;
	}

	if (FightInCondition >= FightsPerCondition)
	{
		ConditionIndex++;
		FightInCondition = 0;
	}

	if (ConditionIndex >= Conditions.Num())
	{
		FinishSimulation();
		return;
	}

	const FSimCondition& C = Conditions[ConditionIndex];

	Director->AdaptationEnabled = C.bAdaptationOn;
	Bot->Policy = C.bRandomPolicy ? EBotPolicy::Random : EBotPolicy::Habitual;
	Bot->Abort();

	// Each fight starts naive. Without this, fight 10 inherits everything fight 1 learned and
	// the runs are not comparable to each other.
	Model->HardReset();

	Recorder->StartFight(GlobalFightIndex, C.bAdaptationOn, C.Name());

	// INTEGRATION POINT — reset the arena. Replace with your own reset:
	//   positions, HP, stamina, boss state (upright), cooldowns, flags, montages.
	if (AOpponentAdapatationGameMode* GameMode = Cast<AOpponentAdapatationGameMode>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AOpponentAdapatationGameMode::StaticClass())))
	{
		GameMode->ResetFight();
	}
	

	UE_LOG(LogTemp, Log, TEXT("SimRunner: fight %d — %s, adaptation %s"),
		GlobalFightIndex, *C.Name(), C.bAdaptationOn ? TEXT("ON") : TEXT("OFF"));

	GetWorld()->GetTimerManager().SetTimer(
		TimeoutTimer, this, &ASimRunner::OnFightTimeout, FightTimeout, false);
}

void ASimRunner::OnFightEnded(bool bPlayerWon)
{
	// top of OnFightEnded


	if (!bRunning)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("SIM: fight ended, won=%d"), bPlayerWon ? 1 : 0);
	GetWorld()->GetTimerManager().ClearTimer(TimeoutTimer);

	Recorder->EndFight(bPlayerWon);
	Bot->Abort();

	FightInCondition++;
	GlobalFightIndex++;

	// One frame of breathing room so death cleanup finishes before the reset runs.
	FTimerHandle NextHandle;
	GetWorld()->GetTimerManager().SetTimer(
		NextHandle, this, &ASimRunner::BeginNextFight, 0.2f, false);
}

void ASimRunner::OnFightTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("SimRunner: fight %d timed out"), GlobalFightIndex);
	OnFightEnded(false);
}

void ASimRunner::AbortSimulation()
{
	if (bRunning)
	{
		FinishSimulation();
	}
}

void ASimRunner::FinishSimulation()
{
	bRunning = false;
	Bot->bEnabled = false;
	GetWorld()->GetTimerManager().ClearTimer(TimeoutTimer);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

	Recorder->LogSummary();
	Recorder->FlushToCsv();

	UE_LOG(LogTemp, Log, TEXT("SimRunner: done — %d fights"), GlobalFightIndex);
}

/* ------------------------------------------------------------------ */
/*  Console commands                                                    */
/* ------------------------------------------------------------------ */

static ASimRunner* FindRunner(UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}
	return Cast<ASimRunner>(
		UGameplayStatics::GetActorOfClass(World, ASimRunner::StaticClass()));
}

static FAutoConsoleCommandWithWorldAndArgs GRunSimCmd(
	TEXT("boss.RunSim"),
	TEXT("boss.RunSim <fightsPerCondition> — runs the adaptation on/off comparison."),
	FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(
		[](const TArray<FString>& Args, UWorld* World)
		{
			if (ASimRunner* Runner = FindRunner(World))
			{
				const int32 N = Args.Num() > 0 ? FCString::Atoi(*Args[0]) : 10;
				Runner->RunSimulation(N);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("No ASimRunner in the level"));
			}
		}));

static FAutoConsoleCommandWithWorld GStopSimCmd(
	TEXT("boss.StopSim"),
	TEXT("Stops the simulation and writes out what has been collected."),
	FConsoleCommandWithWorldDelegate::CreateStatic(
		[](UWorld* World)
		{
			if (ASimRunner* Runner = FindRunner(World))
			{
				Runner->AbortSimulation();
			}
		}));