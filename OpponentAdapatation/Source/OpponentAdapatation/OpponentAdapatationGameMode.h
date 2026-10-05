// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpponentAdapatationGameMode.generated.h"

class AOpponentAdapatationCharacter;
class ACombatEnemy;
/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AOpponentAdapatationGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	TObjectPtr<ACombatEnemy> Boss;
	TObjectPtr<AOpponentAdapatationCharacter> Player;
	
	FVector PlayerStartLocation;
	FRotator PlayerStartRotation;
	FVector BossStartLocation;
	FRotator BossStartRotation;

public:
	
	/** Constructor */
	AOpponentAdapatationGameMode();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void ResetFight() const;
};



