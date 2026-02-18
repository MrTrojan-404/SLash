// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SwapEquippedWeapon.h"

#include "Slash/Interface/CombatInterface.h"

void UAnimNotify_SwapEquippedWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner || !Owner->HasAuthority()) return;

	if (ICombatInterface* Combat = Cast<ICombatInterface>(Owner))
	{
		ICombatInterface::Execute_OnSwapEquippedFinished(Owner);
	}
}
