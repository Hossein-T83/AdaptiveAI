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
	// bound to widgets of the same name in the Blueprint
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> StateText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PredictionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ConfidenceBar;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	TObjectPtr<AActor> EnemyPtr;
	
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void StartTimer(float Time);
	
private:
	float TimerRemaining = 0.f;
};
