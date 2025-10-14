// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Create collision sphere
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	RootComponent = CollisionComponent;

	// Create mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = ProjectileGravityScale;

	InitialLifeSpan = ProjectileLifespan;
}

void AProjectileBase::InitializeProjectile(AActor* InOwner, TSubclassOf<UGameplayEffect> InDamageEffect, float InDamageLevel)
{
	SetOwner(InOwner);
	DamageEffect = InDamageEffect;
	DamageLevel = InDamageLevel;

	// Ignore collision with owner
	if (CollisionComponent && InOwner)
	{
		CollisionComponent->IgnoreActorWhenMoving(InOwner, true);
	}
}

void AProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Don't hit owner
	if (OtherActor == GetOwner())
	{
		return;
	}

	// Check team affiliation - don't hit teammates
	IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(GetOwner());
	IGenericTeamAgentInterface* HitTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);
	
	if (OwnerTeamAgent && HitTeamAgent)
	{
		ETeamAttitude::Type Attitude = OwnerTeamAgent->GetTeamAttitudeTowards(*OtherActor);
		if (Attitude != ETeamAttitude::Hostile)
		{
			return; // Don't damage friendlies or neutrals
		}
	}

	// Apply damage if target has ability system
	if (HasAuthority() && DamageEffect)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC)
		{
			UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
			if (OwnerASC)
			{
				FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
				EffectContext.AddSourceObject(this);
				EffectContext.AddHitResult(Hit);

				FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(DamageEffect, DamageLevel, EffectContext);
				if (SpecHandle.IsValid())
				{
					OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
		}
	}

	// Destroy projectile on hit
	Destroy();
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileBase::OnProjectileHit);
	}
}


