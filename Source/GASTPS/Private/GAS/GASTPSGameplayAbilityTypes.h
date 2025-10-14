// VyniosNikolaos, Thesis, University of Patras, 2025
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GASTPSGameplayAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None							UMETA(DisplayName="None"),
	BasicAttack						UMETA(DisplayName="Basic Attack"),
	Aim								UMETA(DisplayName="Aim"),
	Reload							UMETA(DisplayName="Reload"),  
	AbilityOne						UMETA(DisplayName="Ability One"),
	AbilityTwo						UMETA(DisplayName="Ability Two"),
	Confirm							UMETA(DisplayName="Confirm"),
	Cancel							UMETA(DisplayName="Cancel")
};

