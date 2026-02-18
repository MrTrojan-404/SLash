// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SlashPlayerController.generated.h"

class UCharacterOverlayWidget;
class ASlashHUD;
/**
 * 
 */
UCLASS()
class SLASH_API ASlashPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);

	virtual void OnPossess(APawn* InPawn) override;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void PollInit();
	
private:
	UPROPERTY()
	ASlashHUD* SlashHUD;

	UPROPERTY()
	UCharacterOverlayWidget* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;
	
	float HUDHealth;
	float HUDMaxHealth;
};
