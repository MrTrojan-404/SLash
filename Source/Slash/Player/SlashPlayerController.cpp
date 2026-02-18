// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashPlayerController.h"

#include "SlashCharacter.h"
#include "Components/ProgressBar.h"
#include "Slash/HUD/CharacterOverlayWidget.h"
#include "Slash/HUD/SlashHUD.h"


void ASlashPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SlashHUD = Cast<ASlashHUD>(GetHUD());

	if (SlashHUD && SlashHUD->CharacterOverlay == nullptr)
	{
		SlashHUD->AddCharacterOverlay();
	}
}

void ASlashPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInit();
	
}

void ASlashPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (SlashHUD && SlashHUD->CharacterOverlay)
		{
			//set this so this method is only called once
			CharacterOverlay = SlashHUD->CharacterOverlay;

			if (CharacterOverlay)
			{
				if (bInitializeCharacterOverlay) SetHUDHealth(HUDHealth, HUDMaxHealth);
			}
		}
	}
}

void ASlashPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	SlashHUD = SlashHUD == nullptr ? Cast<ASlashHUD>(GetHUD()) : SlashHUD;

	bool bHUDValid = SlashHUD &&
		SlashHUD->CharacterOverlay &&
		SlashHUD->CharacterOverlay->HealthBar;

	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		SlashHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ASlashPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASlashCharacter* PlayerCharacter = Cast<ASlashCharacter>(InPawn);
	if (PlayerCharacter)
	{
		SetHUDHealth(PlayerCharacter->GetHealth(), PlayerCharacter->GetMaxHealth());
	}
}
