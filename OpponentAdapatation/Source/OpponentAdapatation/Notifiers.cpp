// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifiers.h"

#include "OpponentAdapatationCharacter.h"

void UNotifiers::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            const FAnimNotifyEventReference& EventRef)
{
	Super::Notify(MeshComp, Animation, EventRef);
	if (AOpponentAdapatationCharacter* Character = Cast<AOpponentAdapatationCharacter>(MeshComp->GetOwner()))
	{
		Character->bIsBusy = false;
	}
}

