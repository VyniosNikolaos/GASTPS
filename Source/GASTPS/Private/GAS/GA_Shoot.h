// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "GA_Base.h"
#include "GA_Shoot.generated.h"

/**
 * Ranged shooting ability that fires projectiles towards crosshair
 * Costs mana per shot
 */
UCLASS()
class GASTPS_API UGA_Shoot : public UGA_Base
{
	GENERATED_BODY()

public:
	UGA_Shoot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
		FGameplayTagContainer* OptionalRelevantTags) const override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName ProjectileSpawnSocketName = FName("MuzzleSocket"); 

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ProjectileSpawnDistance = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ShootMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> ManaCostEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Cost")
	float ManaCostAmount = 10.0f;

	void SpawnAndFireProjectile();
	FVector GetCrosshairWorldDirection() const;
};