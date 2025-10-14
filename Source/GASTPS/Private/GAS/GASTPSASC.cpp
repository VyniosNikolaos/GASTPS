// VyniosNikolaos, Thesis, University of Patras, 2025

#include "GASTPSASC.h"
#include "GAS/GASTPSAttributeSet.h"


UGASTPSASC::UGASTPSASC()
{
	GetGameplayAttributeValueChangeDelegate(UGASTPSAttributeSet::GetHealthAttribute()).AddUObject(this, &UGASTPSASC::HealthUpdated);
}

void UGASTPSASC::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		AuthApplyGameplayEffect(EffectClass); 
	}
}

void UGASTPSASC::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TPair<EAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr));
	}

	for (const TPair<EAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}
}

void UGASTPSASC::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStatEffect);
}

void UGASTPSASC::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner()) return;
	if (ChangeData.NewValue <= 0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		AuthApplyGameplayEffect(DeathEffect); 
	}
}

void UGASTPSASC::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}
