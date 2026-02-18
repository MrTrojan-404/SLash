// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IPickupInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	void OnPickupBegin(AActor* PickupActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickup")
	void OnPickupEnd(AActor* PickupActor);
	
};
