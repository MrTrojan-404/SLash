// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackStart.h"

#include "Slash/Component/CombatComponent.h"
#include "Slash/Item/Weapon.h"
#include "Slash/Player/SlashCharacter.h"


void UAnimNotify_AttackStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	
	if (!Owner->HasAuthority()) return;

	if (ASlashCharacter* Character = Cast<ASlashCharacter>(Owner))
	{
		if (UCombatComponent* Combat =
			Character->FindComponentByClass<UCombatComponent>())
		{
			Combat->SetWeaponCollisionEnabled(ECollisionEnabled::QueryOnly);

			if (AWeapon* Weapon = Combat->GetPrimaryEquippedWeapon())
			{
				Weapon->ResetHitActors();
			}
		}
	}
}
