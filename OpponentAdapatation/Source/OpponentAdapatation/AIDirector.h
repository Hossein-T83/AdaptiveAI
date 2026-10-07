// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDirector.generated.h"


struct FGameplayTagContainer;
struct FBossAttack;
class URuleSet;
class UPlayerAdaptation;
struct FFightContext;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPPONENTADAPATATION_API UAIDirector : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIDirector();
	
	UFUNCTION(BlueprintCallable, Category = "Director")
	const FBossAttack ChooseNextAttack(const FGameplayTagContainer& Context);
	UFUNCTION(BlueprintCallable, Category = "Director")
	float CalculateFinalDelay(const FGameplayTagContainer& Context) const;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 IdToIndex(int32 Id) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UPlayerAdaptation> Model;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, Instanced, Category = "Rules")
	TObjectPtr<URuleSet> Rule;
	
	UPROPERTY(EditAnywhere, Category = "Rules")
	float BaseDelay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FBossAttack> ValidAttacks;
	
	TArray<FBossAttack> AttacksOnCooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
	float RecordingWindowTime = 1.5f;
	
	UPROPERTY(EditAnywhere, Category = "Flags")
	bool AdaptationEnabled = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flags")
	bool FightContextOnly = false;
};
