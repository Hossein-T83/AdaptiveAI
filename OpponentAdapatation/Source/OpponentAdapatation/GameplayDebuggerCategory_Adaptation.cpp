#include "GameplayDebuggerCategory_Adaptation.h"

#if WITH_GAMEPLAY_DEBUGGER

#include "PlayerAdaptation.h"
#include "StateTree.h"
#include "Components/StateTreeAIComponent.h"

FGameplayDebuggerCategory_Adaptation::FGameplayDebuggerCategory_Adaptation()
{
	bShowOnlyWithDebugActor = false;
	SetDataPackReplication<FAIData>(&Data);
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_Adaptation::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_Adaptation);
}

void FGameplayDebuggerCategory_Adaptation::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	Data = FAIData();
	if (!DebugActor)
	{
		return;
	}

	const UPlayerAdaptation* Model = DebugActor->FindComponentByClass<UPlayerAdaptation>();
	if (!Model)
	{
		return;
	}
	if (const APawn* Pawn = Cast<APawn>(DebugActor))
	{
		if (AController* C = Pawn->GetController())
		{
			if (auto* STC = C->FindComponentByClass<UStateTreeAIComponent>())
			{
				Data.States = STC->GetActiveStateNames();
			}
		}
	}

	Data.PredictedAction = UEnum::GetValueAsString(Model->GetCurrentPrediction().Action);
	Data.Confidence = Model->GetCurrentPrediction().Confidence;
	Data.Context = Model->GetContextAsString();
	Data.Accuracy = Model->GetAccuracy();
}

void FGameplayDebuggerCategory_Adaptation::DrawData(APlayerController* OwnerPC,
                                                    FGameplayDebuggerCanvasContext& CanvasContext)
{
	CanvasContext.Printf(TEXT("{green}Context: {white}%s"), *Data.Context);
	CanvasContext.Printf(TEXT("{green}Prediction: {white}%s"),
	                     *Data.PredictedAction);
	CanvasContext.Printf(TEXT("{green}Confidence: {yellow}%.0f%%"), Data.Confidence * 100.f);
	CanvasContext.Printf(TEXT("{green}Accuracy: {white}%.0f%%"),
	                     Data.Accuracy * 100.f);

	if (Data.States.Num() > 0)
	{
		CanvasContext.Printf(TEXT("{green}State: {white}%s"), *Data.States.Last().ToString());
	}

	CanvasContext.Printf(TEXT(""));
}

void FGameplayDebuggerCategory_Adaptation::FAIData::Serialize(FArchive& Ar)
{
	Ar << PredictedAction;
	Ar << Confidence;
	Ar << Context;
	Ar << Accuracy;
	Ar << States;
}

#endif
