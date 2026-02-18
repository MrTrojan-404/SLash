// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Slash/Interface/InteractionInterface.h"
#include "Item.generated.h"

class UPickupComponent;
class USphereComponent;

UCLASS()
class SLASH_API AItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionText_Implementation() const override;
	
	void ShowPickupWidget(bool bShow);
	void EnableOutline(bool bEnable);

	void EnableCustomDepth(bool bEnable);

	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPickupComponent* PickupComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* PickupWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category="Interaction")
	class UBoxComponent* TraceBox;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}

