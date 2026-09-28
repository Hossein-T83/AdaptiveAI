// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Notifiers.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Input Open"))
class OPPONENTADAPATATION_API UInputNotify : public UAnimNotify
{
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventRef) override;
	
};

UCLASS(meta = (DisplayName = "Enable Collision"))
class OPPONENTADAPATATION_API UCollisionEnableNotify : public UAnimNotify
{
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventRef) override;
};

UCLASS(meta = (DisplayName = "Disable Collision"))
class OPPONENTADAPATATION_API UCollisionDisableNotify : public UAnimNotify
{
	GENERATED_BODY()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventRef) override;
};
