// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Widgets/OverHeadStatsGauge.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Widgets/ValueGauge.h"
#include "GAS/GASTPSAttributeSet.h"

void UOverHeadStatsGauge::ConfigureWithASC(class UAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UGASTPSAttributeSet::GetHealthAttribute(), UGASTPSAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UGASTPSAttributeSet::GetManaAttribute(), UGASTPSAttributeSet::GetMaxManaAttribute());
	}
}
