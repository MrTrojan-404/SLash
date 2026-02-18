// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystemComponent.h"

#include "Slash/DataAsset/ActionDataAsset.h"


// Sets default values for this component's properties
UActionSystemComponent::UActionSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UActionSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	for (UActionDataAsset* Action : DefaultActions)
	{
		GiveAction(Action);
	}	
}


void UActionSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UActionSystemComponent::GetAvatarActor() const
{
	if (AvatarActor)
	{
		return AvatarActor;
	}
	return GetOwner();
}

void UActionSystemComponent::GrantActiveTags(const FGameplayTagContainer& TagsToAdd)
{
	ActiveGameplayTags.AppendTags(TagsToAdd);
	OnActiveTagsUpdated.Broadcast(ActiveGameplayTags);
}

void UActionSystemComponent::ClearActiveTags(const FGameplayTagContainer& TagsToRemove)
{
	ActiveGameplayTags.RemoveTags(TagsToRemove);
	OnActiveTagsUpdated.Broadcast(ActiveGameplayTags);

}


void UActionSystemComponent::GiveAction(UActionDataAsset* NewAction)
{
	if (!NewAction)
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not Add Action. Action Not Valid"));
		return;
	}

	AvailableActions.Add(NewAction);
}

void UActionSystemComponent::ClearAction(UActionDataAsset* ActionToRemove)
{
	if (!ActionToRemove)
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not Clear Action. Action Not Valid"));
		return;
	}

	AvailableActions.Remove(ActionToRemove);
}

void UActionSystemComponent::AddActiveAction(UActionDataAsset* NewAction)
{
	if (!NewAction)
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not Add Action. Action Not Valid"));
		return;
	}

	ActiveActions.Add(NewAction);
}

void UActionSystemComponent::ClearActiveAction(UActionDataAsset* ActionToRemove)
{
	if (!ActionToRemove)
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not Clear Action. Action Not Valid"));
		return;
	}

	ActiveActions.Remove(ActionToRemove);
}

bool UActionSystemComponent::StartActonByTag(const FGameplayTag ActionTag)
{
	for (UActionDataAsset* Action : AvailableActions)
	{
		if (Action && Action->ActionTag == ActionTag)
		{
			if (!Action->CanStartAction(this)) continue;

			Action->StartAction(this);
			GrantActiveTags(Action->GrantedTags);
			AddActiveAction(Action);
			OnActionStart.Broadcast(ActionTag);
			return true;
		}
	}
	return false;
}

bool UActionSystemComponent::StopActonByTag(const FGameplayTag ActionTag)
{
	for (UActionDataAsset* Action : AvailableActions)
	{
		if (Action && Action->ActionTag == ActionTag)
		{
			if (!Action->CanStopAction(this)) continue;

			Action->StopAction(this);
			ClearActiveTags(Action->GrantedTags);
			ClearActiveAction(Action);
			OnActionStop.Broadcast(ActionTag);
			return true;
		}
	}
	return false;
}

bool UActionSystemComponent::IsActionRunning(const FGameplayTag ActionTag) const
{
	for (const UActionDataAsset* Action : ActiveActions)
	{
		if (Action && Action->ActionTag == ActionTag)
		{
			return true;
		}
	}
	return false;
}
