// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Slash/CombatState.h"
#include "CombatComponent.generated.h"


class ICombatInterface;
class AWeapon;
class AItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLASH_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
						   FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void EquipItem(AItem* ItemToEquip);
	void EquipWeapon(AWeapon* WeaponToEquip);

	void DropEquippedWeapon();
	void Attack();

	void HandleEquippedSwap();
	void EquippedSwap();
	void PlayLocalEquippedSwap();

	void OnAttackFinished();
	void OnEquippedSwapFinished();

	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

#pragma region RPC
	UFUNCTION(Server, Reliable)
	void ServerEquip(AItem* ItemToEquip);
	
	UFUNCTION(Server,Reliable)
	void ServerAttack();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAttackMontage();
	
	UFUNCTION(Server, Reliable)
	void ServerDropWeapon();

	UFUNCTION(Server, Reliable)
	void ServerEquippedSwapWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquippedSwapWeapon();
	
#pragma endregion RPC
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY(ReplicatedUsing = OnRep_PrimaryEquippedWeapon)
	AWeapon* PrimaryEquippedWeapon;
	
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryEquippedWeapon)
	AWeapon* SecondaryEquippedWeapon;

	UPROPERTY(Replicated)
	EEquipState EquipState = EEquipState::ECS_Unequipped;

	UPROPERTY(Replicated)
	EPlayerWeaponState PlayerWeaponState = EPlayerWeaponState::EPWS_HasNoWeapon;
	
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	UFUNCTION()
	void OnRep_PrimaryEquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryEquippedWeapon();
	
	EPlayerWeaponState GetPlayerWeaponState() const;

	void HandleAttack();
	void PlayLocalAttack();

	bool bPredictedAttack = false;
	bool bPredictedSwap = false;

public:
	FORCEINLINE EEquipState GetEquipState() const { return EquipState; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE AWeapon* GetPrimaryEquippedWeapon() const {return  PrimaryEquippedWeapon;};
	
};
