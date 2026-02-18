// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Unequipped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetWeaponState(EWeaponState WeaponStatetoSet);
	void Dropped();
	
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly,  Category = Weapon)
	bool bIsOneHandedWeapon = true;

	void ResetHitActors();
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDamageBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	
private:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;
	
	UPROPERTY(ReplicatedUsing= OnRep_WeaponState)
	EWeaponState WeaponState = EWeaponState::EWS_Initial;

	UFUNCTION()
	void OnRep_WeaponState();

	void OnWeaponStateSet();
	void OnEquipped();
	void OnDropped();
	void OnEquippedSecondary();

	void AttachToOwnerHand();
	void AttachToOwnerBack();

	// prevents multiple hits per swing
	TSet<AActor*> HitActors;
	
public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE float GetDamage() const { return Damage; }

};
