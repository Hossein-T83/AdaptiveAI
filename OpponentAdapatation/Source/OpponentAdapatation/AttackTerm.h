// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AttackTerm.generated.h"

struct FFightContext;
struct FBossAttack;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories)
class OPPONENTADAPATATION_API UAttackTerm : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Score")
	float Score(const FBossAttack& Attack, const FFightContext& Context) const;
	
	virtual float Score_Implementation(const FBossAttack& Attack, const FFightContext& Context) const
	{
		return 1.f;
	}
	
	UPROPERTY(EditAnywhere, Category = "Score")
	FName Name;
	
	UPROPERTY(EditAnywhere, Category = "Score")
	float Weight = 1.f;
};
