// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackFinished.h"
#include "GameFramework/Actor.h"
#include "Slash/Interface/CombatInterface.h"

void UAnimNotify_AttackFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	AActor* Owner = MeshComp->GetOwner();

	if (!Owner->HasAuthority()) return;

	if (Owner->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		// This call handles BOTH state reset and collision disabling via the Character class
		ICombatInterface::Execute_OnAttackFinished(Owner);
	}
}
