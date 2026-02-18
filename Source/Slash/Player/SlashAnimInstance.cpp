// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashAnimInstance.h"
#include "SlashCharacter.h"


void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* PawnOwner = TryGetPawnOwner();
	if (PawnOwner && PawnOwner->GetClass()->ImplementsInterface(UAnimInterface::StaticClass()))
	{
		AnimOwner.SetObject(PawnOwner);
		AnimOwner.SetInterface(Cast<IAnimInterface>(PawnOwner));
	}
}

/*void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!AnimOwner) return;

	APawn* Pawn = Cast<APawn>(AnimOwner.GetObject());
	if (!Pawn) return;
	Velocity = Pawn->GetVelocity();
	Speed = Velocity.Size2D();

	bIsFalling = IAnimInterface::Execute_IsFalling(Pawn);
	bIsCrouched = IAnimInterface::Execute_IsCrouched(Pawn);
	bIsSprinting = IAnimInterface::Execute_IsSprinting(Pawn);

	MovementMode = bIsFalling ? ELocomotionMode::InAir : ELocomotionMode::OnGround;
	Stance = bIsCrouched ? EStance::Crouch : EStance::Stand;
	MovementState = (Speed > 3.f) ? EMovementState::Moving : EMovementState::Idle;

	float WalkSpeed = IAnimInterface::Execute_GetWalkSpeed(Pawn);
	float RunSpeed = IAnimInterface::Execute_GetRunSpeed(Pawn);
	float SprintSpeed = IAnimInterface::Execute_GetSprintSpeed(Pawn);

	// Check For Heavy Land or Light Land
	static bool  bWasFallingLastFrame = false;
	static float LastFallingZSpeed = 0.f;

	const bool bIsFallingNow = bIsFalling;

	bJustLandedLight = false;
	bJustLandedHeavy = false;

	// While falling, keep last Z speed
	if (bIsFallingNow)
	{
		LastFallingZSpeed = Velocity.Z;  // this will be negative while falling
	}

	// Landing frame
	if (bWasFallingLastFrame && !bIsFallingNow)
	{
		const float ImpactSpeed = FMath::Abs(LastFallingZSpeed);

		if (ImpactSpeed > 900.f)
		{
			bJustLandedHeavy = true;
		}
		else if (ImpactSpeed > 300.f) // optional light land min threshold
		{
			bJustLandedLight = true;
		}
	}

	bWasFallingLastFrame = bIsFallingNow;

	// Turn in Place?
	const FRotator ActorRot = Pawn->GetActorRotation();
	const FRotator ControlRot = Pawn->GetControlRotation();

	const float YawDelta = FMath::FindDeltaAngleDegrees(ActorRot.Yaw, ControlRot.Yaw);
	AimYawDelta = YawDelta;

	static bool bWasTurningLastFrame = false;

	if (bWasTurningLastFrame)
	{
		bShouldTurnInPlace = FMath::Abs(YawDelta) > 30.f && !bIsFalling && Speed > 20.f;
	}
	else
	{
		bShouldTurnInPlace = FMath::Abs(YawDelta) > TurnInPlaceYawThreshold && !bIsFalling && Speed > 20.f;
	}

	bWasTurningLastFrame = bShouldTurnInPlace;
	
	// Designer-tunable thresholds (as % of real speeds)
	const float WalkToRunThreshold   = RunSpeed * 0.8f;
	const float RunToSprintThreshold = SprintSpeed * 0.9f;

	if (bIsSprinting)
	{
		Gait = EGait::Sprint;
	}
	else if (Speed >= RunToSprintThreshold)
	{
		Gait = EGait::Run;   // visually still running until sprint anim kicks in
	}
	else if (Speed >= WalkToRunThreshold)
	{
		Gait = EGait::Run;
	}
	else
	{
		Gait = EGait::Walk;
	}

	// Is Starting?
	static bool bWasMovingLastFrame = false;

	bIsStarting = false;

	const bool bHasMovementInput = Pawn->GetLastMovementInputVector().SizeSquared() > 0.01f;
	const bool bIsMovingNow = Speed > 20.f;

	if (!bWasMovingLastFrame && (bHasMovementInput || bIsMovingNow))
	{
		bIsStarting = true;   // one-frame pulse
	}

	bWasMovingLastFrame = bIsMovingNow;

	// Is Pivoting
	static FVector PrevVelocity = FVector::ZeroVector;

	static bool bWasPivotingLastFrame = false;

	bIsPivoting = false;

	if (Speed > 50.f && PrevVelocity.SizeSquared() > 0.f && bHasMovementInput)
	{
		const FVector CurrDir = Velocity.GetSafeNormal2D();
		const FVector PrevDir = PrevVelocity.GetSafeNormal2D();

		const float Dot = FVector::DotProduct(CurrDir, PrevDir);
		const float ClampedDot = FMath::Clamp(Dot, -1.f, 1.f);

		const float Angle = FMath::RadiansToDegrees(FMath::Acos(ClampedDot));

		const bool bPivotNow = Angle > 120.f;

		bIsPivoting = bPivotNow && !bWasPivotingLastFrame;
		bWasPivotingLastFrame = bPivotNow;
	}
	else
	{
		bWasPivotingLastFrame = false; // reset when not eligible
	}

	PrevVelocity = Velocity;

	// Should Spin Transition
	const float SpinYawDelta = FMath::FindDeltaAngleDegrees(
	Pawn->GetActorRotation().Yaw,
	Pawn->GetControlRotation().Yaw
);

	static bool bWasSpinLastFrame = false;

	const bool bSpinNow =
		Speed > 20.f &&
		!bIsFalling &&
		FMath::Abs(SpinYawDelta) > 90.f;

	bShouldSpinTransition = bSpinNow && !bWasSpinLastFrame;

	bWasSpinLastFrame = bSpinNow;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,            // Key: use a fixed key so it updates the same line
			0.f,          // Time: 0 = show for one frame (we refresh every tick)
			FColor::Green,
			FString::Printf(
				TEXT("Start:%d Pivot:%d Spin:%d Turn:%d LandL:%d LandH:%d"),
				bIsStarting ? 1 : 0,
				bIsPivoting ? 1 : 0,
				bShouldSpinTransition ? 1 : 0,
				bShouldTurnInPlace ? 1 : 0,
				bJustLandedLight ? 1 : 0,
				bJustLandedHeavy ? 1 : 0
			)
		);
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			2, // unique key for this line
			0.f,
			FColor::Cyan,
			FString::Printf(
				TEXT("Vel: (X: %.1f, Y: %.1f, Z: %.1f) | Speed: %.1f | InAir: %d"),
				Velocity.X, Velocity.Y, Velocity.Z,
				Speed,
				bIsFalling ? 1 : 0
			)
		);
	}
	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return; // Not in PIE / Standalone game world
	}
	UE_LOG(LogTemp, Log,
		TEXT("Vel=(%.1f, %.1f, %.1f) Speed=%.1f InAir=%d"),
		Velocity.X, Velocity.Y, Velocity.Z,
		Speed,
		bIsFalling ? 1 : 0
	);
	UE_LOG(LogTemp, Log,
		TEXT("Start:%d Pivot:%d Spin:%d Turn:%d LandL:%d LandH:%d"),
		bIsStarting ? 1 : 0,
		bIsPivoting ? 1 : 0,
		bShouldSpinTransition ? 1 : 0,
		bShouldTurnInPlace ? 1 : 0,
		bJustLandedLight ? 1 : 0,
		bJustLandedHeavy ? 1 : 0
	);
}*/

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!AnimOwner) return;

	APawn* Pawn = Cast<APawn>(AnimOwner.GetObject());
	if (!Pawn) return;

	Velocity = Pawn->GetVelocity();
	Speed = Velocity.Size2D();

	bIsFalling   = IAnimInterface::Execute_IsFalling(Pawn);
	bIsCrouched  = IAnimInterface::Execute_IsCrouched(Pawn);
	bIsSprinting = IAnimInterface::Execute_IsSprinting(Pawn);

	MovementMode  = bIsFalling ? ELocomotionMode::InAir : ELocomotionMode::OnGround;
	Stance        = bIsCrouched ? EStance::Crouch : EStance::Stand;
	MovementState = (Speed > 3.f) ? EMovementState::Moving : EMovementState::Idle;

	const float WalkSpeed   = IAnimInterface::Execute_GetWalkSpeed(Pawn);
	const float RunSpeed    = IAnimInterface::Execute_GetRunSpeed(Pawn);
	const float SprintSpeed = IAnimInterface::Execute_GetSprintSpeed(Pawn);

	// -------------------- LANDING (latched)
	static bool  bWasFallingLastFrame = false;
	static float LastFallingZSpeed    = 0.f;
	static float LandTimer            = 0.f;

	bJustLandedLight = false;
	bJustLandedHeavy = false;

	if (bIsFalling)
	{
		LastFallingZSpeed = Velocity.Z;
	}

	if (bWasFallingLastFrame && !bIsFalling)
	{
		const float ImpactSpeed = FMath::Abs(LastFallingZSpeed);

		if (ImpactSpeed > 900.f)
		{
			bJustLandedHeavy = true;
			LandTimer = 0.2f;
		}
		else if (ImpactSpeed > 300.f)
		{
			bJustLandedLight = true;
			LandTimer = 0.15f;
		}
	}

	if (LandTimer > 0.f)
	{
		LandTimer -= DeltaTime;
	}
	else
	{
		bJustLandedLight = false;
		bJustLandedHeavy = false;
	}

	bWasFallingLastFrame = bIsFalling;

	// -------------------- TURN IN PLACE (only when idle)
	const float YawDelta = FMath::FindDeltaAngleDegrees(
		Pawn->GetActorRotation().Yaw,
		Pawn->GetControlRotation().Yaw
	);
	AimYawDelta = YawDelta;

	static bool bWasTurningLastFrame = false;

	const bool bTurnNow =
		!bIsFalling &&
		Speed < 10.f &&
		FMath::Abs(YawDelta) > TurnInPlaceYawThreshold;

	bShouldTurnInPlace = bTurnNow || (bWasTurningLastFrame && FMath::Abs(YawDelta) > 30.f);
	bWasTurningLastFrame = bShouldTurnInPlace;

	// -------------------- GAIT (hysteresis)
	static EGait PrevGait = EGait::Walk;

	const float WalkToRunEnter = RunSpeed * 0.85f;
	const float RunToWalkExit  = RunSpeed * 0.65f;

	if (bIsSprinting)
	{
		Gait = EGait::Sprint;
	}
	else
	{
		switch (PrevGait)
		{
		case EGait::Walk:
			Gait = (Speed > WalkToRunEnter) ? EGait::Run : EGait::Walk;
			break;

		case EGait::Run:
			Gait = (Speed < RunToWalkExit) ? EGait::Walk : EGait::Run;
			break;

		case EGait::Sprint:
			Gait = EGait::Run;
			break;
		}
	}

	PrevGait = Gait;

	// -------------------- STARTING (pulse)
	static bool bWasMovingLastFrame = false;

	const bool bHasMovementInput = Pawn->GetLastMovementInputVector().SizeSquared() > 0.01f;
	const bool bIsMovingNow = Speed > 20.f;

	bIsStarting = !bWasMovingLastFrame && (bHasMovementInput || bIsMovingNow);
	bWasMovingLastFrame = bIsMovingNow;

	// -------------------- PIVOT (pulse)
	static FVector PrevVelocity = FVector::ZeroVector;
	static bool bWasPivotingLastFrame = false;

	bIsPivoting = false;

	if (Speed > 200.f && PrevVelocity.SizeSquared() > 0.f && bHasMovementInput)
	{
		const FVector CurrDir = Velocity.GetSafeNormal2D();
		const FVector PrevDir = PrevVelocity.GetSafeNormal2D();

		const float Dot = FMath::Clamp(FVector::DotProduct(CurrDir, PrevDir), -1.f, 1.f);
		const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

		const bool bPivotNow = Angle > 75.f;
		bIsPivoting = bPivotNow && !bWasPivotingLastFrame;
		bWasPivotingLastFrame = bPivotNow;
	}
	else
	{
		bWasPivotingLastFrame = false;
	}

	PrevVelocity = Velocity;

	// -------------------- SPIN TRANSITION (pulse)
	static bool bWasSpinLastFrame = false;

	const float SpinYawDelta = FMath::FindDeltaAngleDegrees(
		Pawn->GetActorRotation().Yaw,
		Pawn->GetControlRotation().Yaw
	);

	const bool bSpinNow =
		Speed > 150.f &&
		!bIsFalling &&
		FMath::Abs(SpinYawDelta) > 90.f;

	bShouldSpinTransition = bSpinNow && !bWasSpinLastFrame;
	bWasSpinLastFrame = bSpinNow;

	// -------------------- DEBUG (game world only)
	if (UWorld* World = GetWorld(); World && World->IsGameWorld())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1, 0.f, FColor::Green,
				FString::Printf(TEXT("Start:%d Pivot:%d Spin:%d Turn:%d LandL:%d LandH:%d"),
					bIsStarting, bIsPivoting, bShouldSpinTransition,
					bShouldTurnInPlace, bJustLandedLight, bJustLandedHeavy)
			);

			GEngine->AddOnScreenDebugMessage(
				2, 0.f, FColor::Cyan,
				FString::Printf(TEXT("Speed: %.1f  InAir:%d  Gait:%d"),
					Speed, bIsFalling, (int32)Gait)
			);
		}
	}
}
