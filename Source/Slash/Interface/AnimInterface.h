// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slash/CombatState.h"
#include "UObject/Interface.h"
#include "AnimInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IAnimInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	float GetGroundSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	bool IsFalling() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	bool IsCrouched() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	bool IsSprinting() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	float GetWalkSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	float GetRunSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	float GetSprintSpeed() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Animation")
	EEquipState GetEquipState() const;
};
