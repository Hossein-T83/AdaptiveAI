// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDirector.generated.h"


class UPlayerAdaptation;
struct FFightContext;
enum class EBossAttacks : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPPONENTADAPATATION_API UAIDirector : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIDirector();
	
	UFUNCTION(BlueprintCallable, Category = "Director")
	EBossAttacks ChooseNextAttack(const FFightContext& Context);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UPlayerAdaptation> Model;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
