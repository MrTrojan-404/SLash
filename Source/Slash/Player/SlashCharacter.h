// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Slash/Interface/AnimInterface.h"
#include "Slash/Interface/CombatInterface.h"
#include "Slash/Interface/HitInterface.h"
#include "Slash/Interface/PickupInterface.h"
#include "SlashCharacter.generated.h"

class UCameraComponent;
class AItem;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UCombatComponent;
class UInteractionComponent;
class UActionSystemComponent;
class ASlashPlayerController;

UCLASS()
class SLASH_API ASlashCharacter : public ACharacter, public ICombatInterface,
public IPickupInterface, public IAnimInterface, public IHitInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnPickupBegin_Implementation(AActor* PickupActor) override;
	virtual void OnPickupEnd_Implementation(AActor* PickupActor) override;

	
#pragma region RPC

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHitReact(const FVector& ImpactPoint);

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bNewSprinting);

	UFUNCTION(Server, Reliable)
	void ServerSetWalking(bool bNewWalking);
	
#pragma endregion RPC
	
#pragma region HUD

	void UpdateHUDHealth();

#pragma endregion HUD
	
#pragma region AnimInterface
	
	virtual float GetGroundSpeed_Implementation() const override;
	virtual bool IsFalling_Implementation() const override;
	virtual bool IsCrouched_Implementation() const override;
	virtual bool IsSprinting_Implementation() const override;
	virtual EEquipState GetEquipState_Implementation() const override;
	virtual float GetWalkSpeed_Implementation() const override { return WalkSpeed; }
	virtual float GetRunSpeed_Implementation() const override { return RunSpeed; }
	virtual float GetSprintSpeed_Implementation() const override { return SprintSpeed; }
	
#pragma endregion AnimInterface

#pragma region CombatInterface

	virtual USkeletalMeshComponent* GetCombatMesh_Implementation() const override;
	virtual AActor* GetCombatOwner_Implementation() const override;
	
	virtual void PlayAttackMontage_Implementation() override;
	virtual void PlayEquippedSwapMontage_Implementation() override;
	virtual void PlayHitReactMontage_Implementation(const FName& SectionName) override;

	virtual void OnAttackFinished_Implementation() override;
	virtual  void OnSwapEquippedFinished_Implementation() override;

	virtual void AttachToSpringArm_Implementation(AWeapon* WeaponToAttach) override;

	virtual void ReceiveDamage_Implementation(AActor* DamagedActor, float Damage,
		const UDamageType* DamageType, AController* InstigatorController,
		AActor* DamageCauser) override;

	virtual void DirectionalHitReact_Implementation(const FVector& ImpactPoint) override;

#pragma endregion CombatInterface

#pragma region HitInterface

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	
#pragma endregion HitInterface
	
protected:
	virtual void BeginPlay() override;
	
	void PollInit();

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* WeaponSpringArm;

	UPROPERTY(EditAnywhere)
	USceneComponent* WeaponSpringArnSocket;

	
#pragma region  Input
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DropAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* WalkAction;
	
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	
	void InteractButtonPressed();
	void AttackButtonPressed();
	void DropButtonPressed();
	void SwapButtonPressed();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void WalkButtonPressed();
	void WalkButtonReleased();

#pragma endregion Input


private:
	UPROPERTY()
	ASlashPlayerController* SlashPlayerController;
	
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 200;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health;

	UFUNCTION()
	void OnRep_Health(float LastHealth);
	
	void AttackHitStart() const;
	void AttackHitEnd() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UActionSystemComponent* ActionSystemComponent;

	UPROPERTY(Replicated)
	bool bIsSprinting = false;

	UPROPERTY(Replicated)
	bool bIsWalking = false;

	/**
	 * 
	 */
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 700;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 500.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 280.f;
	
#pragma region AnimationMontages

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* SwapMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	
#pragma endregion AnimationMontage

#pragma region Sound

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;
	
#pragma endregion Sound

#pragma region Particles

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;
	
#pragma endregion Particles
	
#pragma region Interaction
	
	void UpdateInteractionFocus();
	void SetFocusedItem(AItem* NewItem);
	void ClearFocusedItem();
	virtual void OnEquipItem_Implementation(AItem* Item) override;

#pragma endregion Interaction

public:
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	
};

