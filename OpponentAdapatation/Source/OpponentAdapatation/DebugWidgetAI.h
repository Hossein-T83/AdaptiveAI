// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugWidgetAI.generated.h"

/**
 * 
 */
UCLASS()
class OPPONENTADAPATATION_API UDebugWidgetAI : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> StateText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PredictionText;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void Refresh(const FString& State, const FString& Prediction, float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	TObjectPtr<AActor> EnemyPtr;
	
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void StartTimer(float Time);
	
private:
	float TimerRemaining = 0.f;
};
