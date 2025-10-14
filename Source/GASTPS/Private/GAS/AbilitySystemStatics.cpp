// VyniosNikolaos, Thesis, University of Patras, 2025

#include "GAS/AbilitySystemStatics.h"

FGameplayTag UAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack");
}

FGameplayTag UAbilitySystemStatics::GetBasicAttackInputPressedTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack.Pressed");
}

FGameplayTag UAbilitySystemStatics::GetBasicAttackInputReleasedTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack.Released");
}

FGameplayTag UAbilitySystemStatics::GetDeadStatTag()
{
	return FGameplayTag::RequestGameplayTag("Status.Dead");
}

FGameplayTag UAbilitySystemStatics::GetAimStatTag()
{
	return FGameplayTag::RequestGameplayTag("Status.Aim");
}


