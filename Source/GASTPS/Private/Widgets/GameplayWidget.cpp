// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Widgets/GameplayWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Widgets/ValueGauge.h"
#include "GAS/GASTPSAttributeSet.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// if ability system component mainly attached to playerstate, get player state instead
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UGASTPSAttributeSet::GetHealthAttribute(), UGASTPSAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UGASTPSAttributeSet::GetManaAttribute(), UGASTPSAttributeSet::GetMaxManaAttribute());
	}
}
