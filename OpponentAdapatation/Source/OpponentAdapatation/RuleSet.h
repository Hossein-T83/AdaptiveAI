// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAdaptation.h"
#include "UObject/Object.h"
#include "RuleSet.generated.h"

struct FGameplayTag;
struct FFightContext;
struct FBossAttack;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories)
class OPPONENTADAPATATION_API URuleSet : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Score")
	float Score(const FFightContext& Context) const;

	virtual float Score_Implementation(const FFightContext& Context) const
	{
		float Result = 1.f;
		for (const auto& Tag : Context.Tags)
		{
			if (const float* Found = Weights.Find(Tag))
			{
				Result *= (*Found);
			}
		}
		return Result;
	}

	UPROPERTY(EditAnywhere, Category = "Rules")
	FName Name;

	UPROPERTY(EditAnywhere, Category = "Rules")
	TMap<FGameplayTag, float> Weights;
};
