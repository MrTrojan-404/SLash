// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SlashPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Slash/Component/ActionSystemComponent.h"
#include "Slash/Component/CombatComponent.h"
#include "Slash/Component/InteractionComponent.h"
#include "Slash/Item/Item.h"
#include "Slash/Item/Weapon.h"


ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->AirControl = 0.7f;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 150.0f;
	SpringArm->SocketOffset.Y = 50.0f;

	WeaponSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("WeaponSpringArm"));
	WeaponSpringArm->SetupAttachment(RootComponent);
	WeaponSpringArm->TargetArmLength = 80.0f;
	
	WeaponSpringArnSocket = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponSocket"));
	WeaponSpringArnSocket->SetupAttachment(WeaponSpringArm);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	ActionSystemComponent = CreateDefaultSubobject<UActionSystemComponent>(TEXT("ActionSystemComponent"));
	ActionSystemComponent->SetIsReplicated(true);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Block);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

}

void ASlashCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASlashCharacter, Health);	
}



void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerControler = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerControler->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ASlashCharacter::ReceiveDamage);
	}

	if (HasAuthority())
	{
		Health = MaxHealth;
	}
}

void ASlashCharacter::PollInit()
{
	if (SlashPlayerController == nullptr)
	{
		SlashPlayerController = SlashPlayerController == nullptr ? Cast<ASlashPlayerController>(Controller) : SlashPlayerController;
		if (SlashPlayerController)
		{
			UpdateHUDHealth();
		}
	}
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInit();
	UpdateInteractionFocus();

}

void ASlashCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
}

void ASlashCharacter::AttackHitStart() const
{
	CombatComponent->SetWeaponCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void ASlashCharacter::AttackHitEnd() const
{
	CombatComponent->SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

#pragma region HUD

void ASlashCharacter::UpdateHUDHealth()
{
	SlashPlayerController = SlashPlayerController == nullptr ? Cast<ASlashPlayerController>(Controller) : SlashPlayerController;
	if (SlashPlayerController)
	{
		SlashPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

#pragma endregion HUD

#pragma region PickupInterface

void ASlashCharacter::OnPickupBegin_Implementation(AActor* PickupActor)
{
	AItem* Item = Cast<AItem>(PickupActor);
	if (!Item || !CombatComponent) return;

	// Entered the item Sphere
	InteractionComponent->NearbyItem = Item;
}

void ASlashCharacter::OnPickupEnd_Implementation(AActor* PickupActor)
{
	AItem* Item = Cast<AItem>(PickupActor);
	if (!Item) return;

	if (InteractionComponent->NearbyItem == Item)
	{
		InteractionComponent->NearbyItem = nullptr;
	}

	if (InteractionComponent->FocusedItem == Item)
	{
		ClearFocusedItem(); // hide widget + remove outline
	}
}

#pragma endregion PickupInterface

#pragma region AnimInterface

float ASlashCharacter::GetGroundSpeed_Implementation() const
{
	return GetVelocity().Size2D();
}

bool ASlashCharacter::IsFalling_Implementation() const
{
	if (GetCharacterMovement())
	{
		return GetCharacterMovement()->IsFalling();
	}
	return false;
}

bool ASlashCharacter::IsCrouched_Implementation() const
{
	return GetCharacterMovement() && GetCharacterMovement()->IsCrouching();
}

bool ASlashCharacter::IsSprinting_Implementation() const
{
	return bIsSprinting; // your own replicated sprint state
}

EEquipState ASlashCharacter::GetEquipState_Implementation() const
{
	if (CombatComponent)
	{
		return CombatComponent->GetEquipState();
	}
	return EEquipState::ECS_Unequipped;
}
#pragma endregion AnimInterface

#pragma region CombatInterface

USkeletalMeshComponent* ASlashCharacter::GetCombatMesh_Implementation() const
{
	return GetMesh();
}

AActor* ASlashCharacter::GetCombatOwner_Implementation() const
{
	return const_cast<ASlashCharacter*>(this);
}

void ASlashCharacter::PlayAttackMontage_Implementation()
{
	if (CombatComponent == nullptr || CombatComponent->GetPrimaryEquippedWeapon() == nullptr) return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (CombatComponent->GetPrimaryEquippedWeapon()->AttackMontage)
		{
			AnimInstance->Montage_Play(CombatComponent->GetPrimaryEquippedWeapon()->AttackMontage);
			FName SectionName = FName("Attack1");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}}

void ASlashCharacter::PlayEquippedSwapMontage_Implementation()
{
	if (CombatComponent == nullptr || CombatComponent->GetPrimaryEquippedWeapon() == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (SwapMontage)
		{
			AnimInstance->Montage_Play(SwapMontage);
		}
	}
}

void ASlashCharacter::PlayHitReactMontage_Implementation(const FName& SectionName)
{
	ICombatInterface::PlayHitReactMontage_Implementation(SectionName);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ASlashCharacter::OnAttackFinished_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->OnAttackFinished();
	}
}

void ASlashCharacter::OnSwapEquippedFinished_Implementation()
{
	ICombatInterface::OnSwapEquippedFinished_Implementation();

	if (CombatComponent)
	{
		CombatComponent->OnEquippedSwapFinished();
	}
}

void ASlashCharacter::AttachToSpringArm_Implementation(AWeapon* WeaponToAttach)
{
	ICombatInterface::AttachToSpringArm_Implementation(WeaponToAttach);

	WeaponToAttach->AttachToComponent(
	WeaponSpringArnSocket,
	FAttachmentTransformRules::SnapToTargetNotIncludingScale
);
	
}

void ASlashCharacter::ReceiveDamage_Implementation(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if (!HasAuthority()) return;

	const float OldHealth = Health;

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	
	UpdateHUDHealth();

	if (Health <= 0.f)
	{
		//Die();
	}
}

void ASlashCharacter::DirectionalHitReact_Implementation(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	ICombatInterface::Execute_PlayHitReactMontage(this, Section);
}


#pragma endregion CombatInterface

#pragma region HitInterface

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	IHitInterface::GetHit_Implementation(ImpactPoint, Hitter);

	// Server triggers multicast
	if (HasAuthority())
	{
		MulticastHitReact(ImpactPoint);
	}
}

#pragma endregion HitInterface

#pragma region Interaction

void ASlashCharacter::UpdateInteractionFocus()
{
	if (!IsLocallyControlled()) return;

	// If nothing nearby, don't trace at all
	if (!InteractionComponent || !InteractionComponent->NearbyItem)
	{
		ClearFocusedItem();
		return;
	}
	
	AController* SlashController = GetController();
	if (!SlashController) return;

	FVector Start;
	FRotator Rot;
	SlashController->GetPlayerViewPoint(Start, Rot);

	FVector End = Start + Rot.Vector() * 500.f;

	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor::Green,
		false,
		0.1f,
		0,
		1.5f
	);
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			0.f,
			FColor::Yellow,
			FString::Printf(TEXT("Trace Hit: %s"),
				bHit ? *GetNameSafe(Hit.GetActor()) : TEXT("NONE"))
		);
	}
	// Only accept if we hit the focused item
	AItem* HitItem = bHit ? Cast<AItem>(Hit.GetActor()) : nullptr;

	// Only focus if the hit item is the one we're near
	if (HitItem && HitItem == InteractionComponent->NearbyItem)
	{
		SetFocusedItem(HitItem);
	}
	else
	{
		ClearFocusedItem();
	}
}

void ASlashCharacter::SetFocusedItem(AItem* NewItem)
{
	if (InteractionComponent->FocusedItem == NewItem) return;
	
	ClearFocusedItem();

	InteractionComponent->FocusedItem = NewItem;

	if (InteractionComponent->FocusedItem)
	{
		InteractionComponent->FocusedItem->ShowPickupWidget(true);
	}
}

void ASlashCharacter::ClearFocusedItem()
{
	if (InteractionComponent->FocusedItem)
	{
		InteractionComponent->FocusedItem->ShowPickupWidget(false);
	}

	InteractionComponent->FocusedItem = nullptr;
}

void ASlashCharacter::OnEquipItem_Implementation(AItem* Item)
{
	if (!CombatComponent || !Item) return;

	CombatComponent->ServerEquip(Item);
}

#pragma endregion Interaction

#pragma region RPC

void ASlashCharacter::MulticastHitReact_Implementation(const FVector& ImpactPoint)
{
	ICombatInterface::Execute_DirectionalHitReact(this, ImpactPoint);
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void ASlashCharacter::ServerSetSprinting_Implementation(bool bNewSprinting)
{
	bIsSprinting = bNewSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : RunSpeed;
}

void ASlashCharacter::ServerSetWalking_Implementation(bool bNewWalking)
{
	bIsWalking = bNewWalking;
	GetCharacterMovement()->MaxWalkSpeed = bIsWalking ? WalkSpeed : RunSpeed;
}

#pragma endregion RPC

#pragma region  Input

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::AttackButtonPressed);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASlashCharacter::InteractButtonPressed);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &ASlashCharacter::DropButtonPressed);
		EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Triggered, this, &ASlashCharacter::SwapButtonPressed);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ASlashCharacter::SprintButtonPressed);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASlashCharacter::SprintButtonReleased);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &ASlashCharacter::WalkButtonPressed);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &ASlashCharacter::WalkButtonReleased);
		//EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Crouch);

	}
}


void ASlashCharacter::Move(const FInputActionValue& value)
{
	if (Controller != nullptr)
	{
		// Forward and Backward Movement Input
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.0f);
		const FVector ForwardDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(ForwardDirection, value.Get<FVector>().Y);

		// Left and Right Movement
		const FVector RightDirection(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(RightDirection, value.Get<FVector>().X);
	}
}

void ASlashCharacter::Look(const FInputActionValue& value)
{
	AddControllerPitchInput(value.Get<FVector2d>().Y);
	AddControllerYawInput(value.Get<FVector2d>().X);
}

void ASlashCharacter::InteractButtonPressed()
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

void ASlashCharacter::AttackButtonPressed()
{
	if (CombatComponent)
	{
		CombatComponent->Attack(); 
	}
}

void ASlashCharacter::DropButtonPressed()
{
	if (CombatComponent)
	{
		CombatComponent->ServerDropWeapon();
	}
}

void ASlashCharacter::SwapButtonPressed()
{
	if (CombatComponent)
	{
		CombatComponent->EquippedSwap();
	}
}

void ASlashCharacter::SprintButtonPressed()
{
	if (bIsWalking) return; // walking cancels sprint

	if (HasAuthority())
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		ServerSetSprinting(true);
	}
}

void ASlashCharacter::SprintButtonReleased()
{
	if (HasAuthority())
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		ServerSetSprinting(false);
	}
}

void ASlashCharacter::WalkButtonPressed()
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
		ServerSetSprinting(false);
	}
	if (HasAuthority())
	{
		bIsWalking = true;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		ServerSetWalking(true);
	}
}

void ASlashCharacter::WalkButtonReleased()
{
	if (HasAuthority())
	{
		bIsWalking = false;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		ServerSetWalking(false);
	}
}

#pragma endregion Input
