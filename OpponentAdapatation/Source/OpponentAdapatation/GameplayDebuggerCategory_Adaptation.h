#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"

class FGameplayDebuggerCategory_Adaptation : public FGameplayDebuggerCategory
{
	public:
	FGameplayDebuggerCategory_Adaptation();
	
	static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
	
	virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;
	
	protected:
	struct FAIData
	{
		FString PredictedAction;
		float Accuracy;
		float Confidence;
		FString Context;
		
		void Serialize(FArchive& Ar);
	};
	
	FAIData Data;
	
};

#endif





