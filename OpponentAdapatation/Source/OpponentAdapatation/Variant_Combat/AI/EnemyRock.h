// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Variant_Combat/AI/CombatEnemy.h"
#include "EnemyRock.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	HammerAttack,
	RollAttack,
	CircleAttack
};

enum EEnemyState
{
	Toppled,
	Standing,
	Rising,
	Rolling,
	Waiting
};

UCLASS()
class OPPONENTADAPATATION_API AEnemyRock : public ACombatEnemy
{
	GENERATED_BODY()
	
public:
	AEnemyRock();
	virtual void Tick(float DeltaTime);
	EEnemyState GetCurrentState() const;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Combat")
	float ToppleDuration = 1.f;
	UPROPERTY(BlueprintReadWrite, Category="Combat")
	EAttackType CurrentAttackType;
	
private:
	void HammerAttack() const;
	void RollAttack() const;
	
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	float ToppleAlpha = 0.f;
	FRotator RiseAxis = FRotator(0.0f, 0.0f, 0.0f);
	float DelayTimer = 0.f;
	EEnemyState CurrentState = EEnemyState::Standing;
	
};
