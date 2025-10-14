// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class UAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FGameplayTag GetBasicAttackAbilityTag();
	static FGameplayTag GetBasicAttackInputPressedTag();
	static FGameplayTag GetBasicAttackInputReleasedTag();
	static FGameplayTag GetDeadStatTag();
	static FGameplayTag GetAimStatTag();
};
