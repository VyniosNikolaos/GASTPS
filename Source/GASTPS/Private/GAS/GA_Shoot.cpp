// Vynios Nikolaos, 2025, University of Patras, Thesis

#include "GA_Shoot.h"
#include "AbilitySystemStatics.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ProjectileBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Player/GASTPSPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GASTPSAttributeSet.h"

UGA_Shoot::UGA_Shoot()
{
	AbilityTags.AddTag(UAbilitySystemStatics::GetBasicAttackAbilityTag());
	BlockAbilitiesWithTag.AddTag(UAbilitySystemStatics::GetBasicAttackAbilityTag());
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Shoot::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Check if we have enough mana
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		const UGASTPSAttributeSet* Attributes = ASC->GetSet<UGASTPSAttributeSet>();
		if (Attributes)
		{
			float CurrentMana = Attributes->GetMana();
			if (CurrentMana < ManaCostAmount)
			{
				return false; // Not enough mana
			}
		}
	}

	return true;
}

void UGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Commit ability (costs, cooldowns, etc.)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Apply mana cost on server
	if (K2_HasAuthority() && ManaCostEffect)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ManaCostEffect, GetAbilityLevel(), EffectContext);
			if (SpecHandle.IsValid())
			{
				// Set the mana cost magnitude
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.ManaCost"), ManaCostAmount);
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}

	// Play shoot montage if available
	if (ShootMontage && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayShootMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, ShootMontage, 1.0f);
		
		PlayShootMontageTask->OnBlendOut.AddDynamic(this, &UGA_Shoot::K2_EndAbility);
		PlayShootMontageTask->OnCancelled.AddDynamic(this, &UGA_Shoot::K2_EndAbility);
		PlayShootMontageTask->OnCompleted.AddDynamic(this, &UGA_Shoot::K2_EndAbility);
		PlayShootMontageTask->OnInterrupted.AddDynamic(this, &UGA_Shoot::K2_EndAbility);
		PlayShootMontageTask->ReadyForActivation();
	}
	else
	{
		// No montage, end ability immediately after spawning projectile
		K2_EndAbility();
	}

	// Spawn projectile
	SpawnAndFireProjectile();
}

void UGA_Shoot::SpawnAndFireProjectile()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Shoot: ProjectileClass is null!"));
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority())
	{
		return; // Only spawn on server
	}

    // Get crosshair direction for projectile trajectory
	FVector Direction = GetCrosshairWorldDirection();

	// Try to find the spawn socket on the character's mesh
	FVector SpawnLocation;
	FRotator SpawnRotation = Direction.Rotation();

	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (Character && Character->GetMesh())
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
        
		// Try to get socket location 
		if (Mesh->DoesSocketExist(ProjectileSpawnSocketName))
		{
			SpawnLocation = Mesh->GetSocketLocation(ProjectileSpawnSocketName);
		}
		else
		{
			// Fallback: Use character's eye height + forward offset
			SpawnLocation = Character->GetPawnViewLocation() + Direction * ProjectileSpawnDistance;
			UE_LOG(LogTemp, Warning, TEXT("GA_Shoot: Socket '%s' not found, using view location fallback"), 
				*ProjectileSpawnSocketName.ToString());
		}
	}
	else
	{
		// Ultimate fallback
		SpawnLocation = AvatarActor->GetActorLocation() + FVector(0, 0, 88.0f) + Direction * ProjectileSpawnDistance;
	}

	// Spawn projectile
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(AvatarActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
		ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		Projectile->InitializeProjectile(AvatarActor, DamageEffect, GetAbilityLevel());
	}
}

FVector UGA_Shoot::GetCrosshairWorldDirection() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return FVector::ForwardVector;
	}

	// Try to get camera component from character
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (Character)
	{
		// Find camera component
		UCameraComponent* CameraComponent = Character->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			// Fire in the direction camera is looking (crosshair is at screen center)
			return CameraComponent->GetForwardVector();
		}
	}

	// Fallback to actor forward vector
	return AvatarActor->GetActorForwardVector();
}