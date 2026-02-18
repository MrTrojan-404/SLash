// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashHUD.h"

#include "CharacterOverlayWidget.h"
#include "Blueprint/UserWidget.h"

void ASlashHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlayWidget>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ASlashHUD::DrawHUD()
{
	Super::DrawHUD();
	
}
