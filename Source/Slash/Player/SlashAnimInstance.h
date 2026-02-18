// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Slash/CombatState.h"
#include "Slash/Interface/AnimInterface.h"
#include "SlashAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ELocomotionMode : uint8
{
	OnGround UMETA(DisplayName = "OnGround"),
	InAir    UMETA(DisplayName = "InAir")
};

UENUM(BlueprintType)
enum class EStance : uint8
{
	Stand  UMETA(DisplayName = "Stand"),
	Crouch UMETA(DisplayName = "Crouch")
};
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle   UMETA(DisplayName = "Idle"),
	Moving UMETA(DisplayName = "Moving")
};

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walk   UMETA(DisplayName = "Walk"),
	Run    UMETA(DisplayName = "Run"),
	Sprint UMETA(DisplayName = "Sprint")
};

class UCombatComponent;
/**
 * 
 */
UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly)
	bool bJustLandedLight = false;

	UPROPERTY(BlueprintReadOnly)
	bool bJustLandedHeavy = false;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldTurnInPlace = false;

	UPROPERTY(BlueprintReadOnly)
	float Speed2D = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	ELocomotionMode MovementMode;   // OnGround / InAir

	UPROPERTY(BlueprintReadOnly)
	EMovementState MovementState;     // Idle / Moving

	UPROPERTY(BlueprintReadOnly)
	EStance Stance;					 // Crouch / Stand
	
	UPROPERTY(BlueprintReadOnly)
	EGait Gait;                       // Walk / Run / Sprint
	
	UPROPERTY(BlueprintReadOnly, Category = Character)
	EEquipState EquipState;

	UPROPERTY(BlueprintReadOnly)
	float AimYawDelta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Locomotion|Turn")
	float TurnInPlaceYawThreshold = 45.f;

	UPROPERTY(BlueprintReadOnly)
	bool bJustTraversed = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsStarting;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsPivoting;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldSpinTransition;

private:
	UPROPERTY()
	TScriptInterface<IAnimInterface> AnimOwner;
	
};
