// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackTerm.h"
#include "DistanceTerm.generated.h"

/**
 * 
 */
UCLASS()
class OPPONENTADAPATATION_API UDistanceTerm : public UAttackTerm
{
	GENERATED_BODY()
public:
	float Score(const FBossAttack& Attack, const FFightContext& Context) const;
	virtual float Score_Implementation(const FBossAttack& Attack, const FFightContext& Context) const override;
	
};
