// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Slash/WeaponTypes.h"
#include "Slash/Component/PickupComponent.h"
#include "Slash/Interface/CombatInterface.h"
#include "Slash/Interface/HitInterface.h"
#include "Slash/Player/SlashCharacter.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponBox->SetCollisionObjectType(ECC_WorldDynamic);

	WeaponBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnDamageBoxOverlap);

	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		WeaponBox->IgnoreActorWhenMoving(PawnOwner, true);
	}
}


void AWeapon::OnDamageBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == GetOwner()) return;

	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	FHitResult BoxHit;
	UKismetSystemLibrary::BoxTraceSingle(
			this, Start, End, FVector(5.f, 5.f, 5.f),
			BoxTraceStart->GetComponentRotation(),
			UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), // Custom Weapon Channel
			false, ActorsToIgnore, EDrawDebugTrace::ForDuration,
			BoxHit, true
		);

	if (OtherActor == GetOwner())
	{
		return;
	}
	
	AActor* HitActor = BoxHit.GetActor();
	if (!HitActor) return;

	if (HitActor == GetOwner()) return;

	if (HitActors.Contains(HitActor)) return;
	HitActors.Add(HitActor);

	UGameplayStatics::ApplyDamage(
		HitActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	// Visual Only
	if (HitActor->Implements<UHitInterface>())
	{
		IHitInterface::Execute_GetHit(HitActor, BoxHit.ImpactPoint, GetOwner());
	}
}


#pragma region Handle Weapon States

void AWeapon::SetWeaponState(EWeaponState WeaponStatetoSet)
{
	WeaponState = WeaponStatetoSet;
	OnWeaponStateSet();
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	ItemMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
}

void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		OnEquipped();
		break;
	case EWeaponState::EWS_EquippedSecondary:
		OnEquippedSecondary();
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	}
}

void AWeapon::OnDropped()
{
	PickupComponent->PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetEnableGravity(true);
	SetReplicateMovement(true);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	TraceBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TraceBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TraceBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	ItemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	ItemMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

}

void AWeapon::OnEquipped()
{
	ShowPickupWidget(false);
	PickupComponent->PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetEnableGravity(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TraceBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TraceBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	AttachToOwnerHand();

	if (AActor* OwnerActor = GetOwner())
	{
		WeaponBox->IgnoreActorWhenMoving(OwnerActor, true);
	}
	
	EnableCustomDepth(false);
}

void AWeapon::OnEquippedSecondary()
{
	ShowPickupWidget(false);
	PickupComponent->PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetEnableGravity(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TraceBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TraceBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	AttachToOwnerBack();

	if (AActor* OwnerActor = GetOwner())
	{
		WeaponBox->IgnoreActorWhenMoving(OwnerActor, true);
	}
	
	EnableCustomDepth(true);
	
	ItemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	ItemMesh->MarkRenderStateDirty();
}

#pragma  endregion Handle Weapon States

#pragma region Helper Functions

void AWeapon::AttachToOwnerHand()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	ICombatInterface* CombatOwner = Cast<ICombatInterface>(OwnerActor);
	if (!CombatOwner) return;

	USkeletalMeshComponent* Mesh =
		ICombatInterface::Execute_GetCombatMesh(OwnerActor);

	if (!Mesh) return;

	const USkeletalMeshSocket* RightHandSocket =
		Mesh->GetSocketByName("RightHandSocket");

	if (RightHandSocket)
	{
		RightHandSocket->AttachActor(this, Mesh);
	}
}

void AWeapon::AttachToOwnerBack()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	ICombatInterface* CombatOwner = Cast<ICombatInterface>(OwnerActor);
	if (!CombatOwner) return;

	ICombatInterface::Execute_AttachToSpringArm(OwnerActor, this);
}
void AWeapon::ResetHitActors()
{
	HitActors.Empty();
}

#pragma endregion Helper Functions