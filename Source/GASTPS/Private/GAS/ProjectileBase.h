// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "ProjectileBase.generated.h"

UCLASS()
class GASTPS_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	// Initialize projectile with owner info for damage dealing
	void InitializeProjectile(AActor* InOwner, TSubclassOf<UGameplayEffect> InDamageEffect, float InDamageLevel);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ProjectileSpeed = 3000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ProjectileGravityScale = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ProjectileLifespan = 10.0f;

	// Damage info
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffect;

	float DamageLevel;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		FVector NormalImpulse, const FHitResult& Hit);
};
