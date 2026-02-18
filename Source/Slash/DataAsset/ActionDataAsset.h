// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ActionDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FActionMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Action")
	UAnimMontage* ActionMontage;

	UPROPERTY(EditAnywhere, Category = "Action")
	float PlayRate;

	UPROPERTY(EditAnywhere, Category = "Action")
	bool bRandomMontageSection;

	UPROPERTY(EditAnywhere, Category = "Action")
	TArray<FName> MontageSectionNames;

	FActionMontage()
		:ActionMontage(nullptr),
		PlayRate(1.0f),
		bRandomMontageSection(false),
		MontageSectionNames({})
	{}
		
	bool IsMontageValid() const
	{
		return ActionMontage != nullptr;
	}

	int32 GetLastSectionIndex() const
	{
		return MontageSectionNames.Num() - 1;
	}

	int32 GetRandSectionIndex() const
	{
		if (MontageSectionNames.IsEmpty())
		{
			return 0;
		}

		int32 RandIndex = FMath::RandRange(0, GetLastSectionIndex());
		if (!MontageSectionNames.IsValidIndex(RandIndex))
		{
			return -1;
		}
		return RandIndex;
	}

	FName GetMontageSectionName(const int32 SectionIndex) const
	{
		if (MontageSectionNames.IsEmpty())
		{
			return NAME_None;
		}
		int32 SelectedIndex = SectionIndex;
		if (bRandomMontageSection)
		{
			SelectedIndex = GetRandSectionIndex();
		}
		if (!MontageSectionNames.IsValidIndex(SelectedIndex))
		{
			return NAME_None;
		}
		return MontageSectionNames[SectionIndex];
	}
	
};

class UActionSystemComponent;

UCLASS()
class SLASH_API UActionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//Action tag to find an action by tag without a referenceto the object
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	FGameplayTag ActionTag;

	//Tags added to owning Actor when action starts removed when action stops
	UPROPERTY(EditAnywhere, Category = "Action")
	FGameplayTagContainer GrantedTags;

	// Action Can Only Start if owning actor has none of these tags
	UPROPERTY(EditAnywhere, Category = "Action")
	FGameplayTagContainer BlockedTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	FActionMontage ActionMontage;

	virtual void StartAction(UActionSystemComponent* ActionComponent);
	virtual void StopAction(UActionSystemComponent* ActionComponent);
	virtual bool CanStartAction(UActionSystemComponent* ActionComponent) const;
	virtual bool CanStopAction(UActionSystemComponent* ActionComponent) const;
	bool DoesSatisfyTagRequirements(const FGameplayTagContainer& ActiveGameplayTags) const;
};
