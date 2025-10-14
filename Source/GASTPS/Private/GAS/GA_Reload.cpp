// Vynios Nikolaos, 2025, University of Patras, Thesis

#include "GA_Reload.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GASTPSAttributeSet.h"

UGA_Reload::UGA_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Play reload montage if available
	if (ReloadMontage && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayReloadMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, ReloadMontage, 1.0f);
		
		PlayReloadMontageTask->OnBlendOut.AddDynamic(this, &UGA_Reload::K2_EndAbility);
		PlayReloadMontageTask->OnCancelled.AddDynamic(this, &UGA_Reload::K2_EndAbility);
		PlayReloadMontageTask->OnCompleted.AddDynamic(this, &UGA_Reload::K2_EndAbility);
		PlayReloadMontageTask->OnInterrupted.AddDynamic(this, &UGA_Reload::K2_EndAbility);
		PlayReloadMontageTask->ReadyForActivation();
	}

	// Wait for reload duration before restoring mana
	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, ReloadDuration);
	WaitTask->OnFinish.AddDynamic(this, &UGA_Reload::OnReloadComplete);
	WaitTask->ReadyForActivation();
}

void UGA_Reload::OnReloadComplete()
{
	// Restore mana on server
	if (K2_HasAuthority() && ManaRestoreEffect)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ManaRestoreEffect, GetAbilityLevel(), EffectContext);
			if (SpecHandle.IsValid())
			{
				// Set the mana restore magnitude
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.ManaRestore"), ManaRestoreAmount);
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}

	// End ability if no montage (montage will end it automatically)
	if (!ReloadMontage)
	{
		K2_EndAbility();
	}
}