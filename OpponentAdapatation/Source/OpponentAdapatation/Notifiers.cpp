// Fill out your copyright notice in the Description page of Project Settings.


#include "Notifiers.h"

#include "OpponentAdapatationCharacter.h"


void UInputNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventRef)
{
	Super::Notify(MeshComp, Animation, EventRef);
	if (AOpponentAdapatationCharacter* Character = Cast<AOpponentAdapatationCharacter>(MeshComp->GetOwner()))
	{
		Character->bIsBusy = false;
	}
}

void UCollisionEnableNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventRef)
{
	Super::Notify(MeshComp, Animation, EventRef);
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	TArray<UPrimitiveComponent*> Comps;
	Owner->GetComponents<UPrimitiveComponent>(Comps);

	for (UPrimitiveComponent* C : Comps)
	{
		if (C->GetName() == TEXT("WeaponHitBox"))
		{
			C->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			C->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
			break;
		}
	}
}

void UCollisionDisableNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventRef)
{
	Super::Notify(MeshComp, Animation, EventRef);
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	TArray<UPrimitiveComponent*> Comps;
	Owner->GetComponents<UPrimitiveComponent>(Comps);

	for (UPrimitiveComponent* C : Comps)
	{
		if (C->GetName() == TEXT("WeaponHitBox"))
		{
			C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		}
	}
}
