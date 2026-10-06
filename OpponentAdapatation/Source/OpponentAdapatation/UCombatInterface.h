#pragma once

#include "CoreMinimal.h"
#include "UCombatInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class ICombatInterface
{
	GENERATED_BODY()
	public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = CombatInterface)
	void TakeDamage(const float Damage);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = CombatInterface)
	void HandleOverlapping(AActor* OverlappedActor);
};