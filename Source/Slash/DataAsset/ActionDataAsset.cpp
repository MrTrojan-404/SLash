// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionDataAsset.h"

void UActionDataAsset::StartAction(UActionSystemComponent* ActionComponent)
{
}

void UActionDataAsset::StopAction(UActionSystemComponent* ActionComponent)
{
}

bool UActionDataAsset::CanStartAction(UActionSystemComponent* ActionComponent) const
{
	return false;
}

bool UActionDataAsset::CanStopAction(UActionSystemComponent* ActionComponent) const
{
	return false;
}

bool UActionDataAsset::DoesSatisfyTagRequirements(const FGameplayTagContainer& ActiveGameplayTags) const
{
	return !ActiveGameplayTags.HasAny(BlockedTags);
}

