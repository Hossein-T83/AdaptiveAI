// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "InputNotifier.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Input Open"))
class OPPONENTADAPATATION_API UInputNotifier : public UAnimNotify
{
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventRef) override;
	
};
