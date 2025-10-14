// Vynios Nikolaos, 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "GA_Base.h"
#include "GA_Reload.generated.h"

/**
 * Reload ability that restores mana
 */
UCLASS()
class GASTPS_API UGA_Reload : public UGA_Base
{
	GENERATED_BODY()

public:
	UGA_Reload();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> ManaRestoreEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	float ManaRestoreAmount = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	float ReloadDuration = 2.0f; 

	UFUNCTION()
	void OnReloadComplete();
};