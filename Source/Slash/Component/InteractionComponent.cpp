#include "InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Slash/Interface/InteractionInterface.h"
#include "Slash/Item/Item.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerActor = GetOwner();
}

void UInteractionComponent::TryInteract()
{
	if (!FocusedItem) return;
	
	ServerInteract(FocusedItem);
}

void UInteractionComponent::ServerInteract_Implementation(AActor* TargetActor)
{
	if (!TargetActor || !OwnerActor) return;

	// Must be interactable
	if (!TargetActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		return;

	// Must be close enough
	const float Distance = FVector::Dist(
		OwnerActor->GetActorLocation(),
		TargetActor->GetActorLocation()
	);

	if (Distance > 200.f) return;

	// Optional: line of sight check
	FHitResult Hit;
	FVector Start = OwnerActor->GetActorLocation();
	FVector End = TargetActor->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params
	);

	if (bHit && Hit.GetActor() != TargetActor)
		return; // blocked by wall or object

	// Server approves interaction
	IInteractionInterface::Execute_Interact(TargetActor, OwnerActor);
}
