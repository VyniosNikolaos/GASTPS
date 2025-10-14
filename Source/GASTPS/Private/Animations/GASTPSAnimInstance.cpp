// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Animations/GASTPSAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/AbilitySystemStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/GASTPSPlayerCharacter.h"

void UGASTPSAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = Cast<ACharacter>(GetOwningActor());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	if (OwnerAsc)
	{
		OwnerAsc->RegisterGameplayTagEvent(UAbilitySystemStatics::GetAimStatTag()).AddUObject(this, &UGASTPSAnimInstance::OwnerAimTagUpdated);
	}
}

void UGASTPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();
		FRotator BodyRot = OwnerCharacter->GetActorRotation();
		FRotator BodyRotDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, BodyPrevRot);
		BodyPrevRot = BodyRot;

		YawSpeed = BodyRotDelta.Yaw / DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(
			SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSmoothLerpSpeed);
		FRotator ControlRot = OwnerCharacter->GetBaseAimRotation(); // in engine implementation pitch is replicated but yaw is not when pawn doesnt have a controller...bad for multiplayer
		LookRotOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);

		FwdSpeed = Velocity.Dot(ControlRot.Vector());
		RightSpeed = -Velocity.Dot(ControlRot.Vector().Cross(FVector::UpVector));	
	}

	if (OwnerMovementComp)
	{
		bIsJumping = OwnerMovementComp->IsFalling();
	}
}

void UGASTPSAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	
}

bool UGASTPSAnimInstance::ShouldDoFullBody() const
{
	return (GetSpeed() <= 0.0f) && !(GetIsAiming());
}

void UGASTPSAnimInstance::OwnerAimTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount > 0;
}
