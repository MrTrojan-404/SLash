#include "PickupComponent.h"

#include "Components/SphereComponent.h"
#include "Slash/Interface/PickupInterface.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();
	if (!OwnerActor) return;

	// Create sphere dynamically and attach to root
	PickupSphere = NewObject<USphereComponent>(OwnerActor);
	PickupSphere->RegisterComponent();
	PickupSphere->AttachToComponent(OwnerActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	PickupSphere->SetSphereRadius(160);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::OnSphereOverlap);
	PickupSphere->OnComponentEndOverlap.AddDynamic(this, &UPickupComponent::OnSphereEndOverlap);
}

void UPickupComponent::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	if (OtherActor->GetClass()->ImplementsInterface(UPickupInterface::StaticClass()))
	{
		IPickupInterface::Execute_OnPickupBegin(OtherActor, GetOwner());
	}
}

void UPickupComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;
	
	if (OtherActor->GetClass()->ImplementsInterface(UPickupInterface::StaticClass()))
	{
		IPickupInterface::Execute_OnPickupEnd(OtherActor, GetOwner());
	}
}