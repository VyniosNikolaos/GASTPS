// VyniosNikolaos, Thesis, University of Patras, 2025
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GAS/GASTPSGameplayAbilityTypes.h"
#include "GASTPSASC.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASTPS_API UGASTPSASC : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGASTPSASC();
	
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	void ApplyFullStatEffect();
	
private:
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<EAbilityInputID ,TSubclassOf<UGameplayAbility>> Abilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<EAbilityInputID ,TSubclassOf<UGameplayAbility>> BasicAbilities;


};
