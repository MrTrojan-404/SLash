// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class AItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLASH_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	void TryInteract();

	UPROPERTY()
	AItem* FocusedItem;

	UPROPERTY()
	AItem* NearbyItem;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY(EditAnywhere)
	float TraceDistance = 250.f;

	// Server RPC
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* TargetActor);
	
};
