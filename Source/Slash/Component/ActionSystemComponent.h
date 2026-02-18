// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ActionSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionStart, FGameplayTag, ActionTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionStop, FGameplayTag, ActionTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveTagsUpdated,FGameplayTagContainer, ActiveGameplayTags);

class UActionDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLASH_API UActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActionSystemComponent();

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnActionStart OnActionStart;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnActionStop OnActionStop;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnActiveTagsUpdated OnActiveTagsUpdated;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UFUNCTION(BlueprintCallable, Category = "Action System")
	void GrantActiveTags(const FGameplayTagContainer& TagsToAdd);

	UFUNCTION(BlueprintCallable, Category = "Action System")
	void ClearActiveTags(const FGameplayTagContainer& TagsToRemove);

	UFUNCTION(BlueprintCallable, Category = "Action System")
	void GiveAction(UActionDataAsset* NewAction);

	UFUNCTION(BlueprintCallable, Category = "Action System")
	void ClearAction(UActionDataAsset* ActionToRemove);

	UFUNCTION(Category = "Action System")
	void AddActiveAction(UActionDataAsset* NewAction);

	UFUNCTION(Category = "Action System")
	void ClearActiveAction(UActionDataAsset* ActionToRemove);

	UFUNCTION(BlueprintCallable, Category = "Action System")
	bool StartActonByTag(const FGameplayTag ActionTag);

	UFUNCTION(BlueprintCallable, Category = "Action System")
	bool StopActonByTag(const FGameplayTag ActionTag);	
	
	UFUNCTION(BlueprintCallable, Category = "Action System")
	bool IsActionRunning(const FGameplayTag ActionTag) const;

	UFUNCTION(BlueprintCallable, Category = "Action System")
	void SetAvatarActor(AActor* InActor){AvatarActor = InActor;}
	
	UFUNCTION(BlueprintCallable, Category = "Action System")
	AActor* GetAvatarActor() const;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<TObjectPtr<UActionDataAsset>> AvailableActions;

	UPROPERTY()
	TArray<TObjectPtr<UActionDataAsset>> ActiveActions;

	UPROPERTY(EditDefaultsOnly, Category="Action System")
	TArray<TObjectPtr<UActionDataAsset>>  DefaultActions;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> AvatarActor;
	
};
