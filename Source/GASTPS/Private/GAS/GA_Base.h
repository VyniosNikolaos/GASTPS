// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "GA_Base.generated.h"

/**
 * 
 */
UCLASS()
class UGA_Base : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	class UAnimInstance* GetOwnerAnimInstance() const;
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
		 float SphereSweepRadius = 30.f,ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, bool bIgnoreSelf=true) const;
	
};
