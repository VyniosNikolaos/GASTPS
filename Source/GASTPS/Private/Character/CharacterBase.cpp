// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Character/CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "Framework/GASTPSGameState.h"
#include "GAS/GASTPSASC.h"
#include "GAS/GASTPSAttributeSet.h"
#include "GAS/AbilitySystemStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/OverHeadStatsGauge.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GASTPSASC = CreateDefaultSubobject<UGASTPSASC>("Ability System Component");
	AttributeSet = CreateDefaultSubobject<UGASTPSAttributeSet>("Attribute Set");
	
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Over Head Widget Component");
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());

	BindGASChangeDelegates();
}

void ACharacterBase::ServerSideInit()
{
	GASTPSASC->InitAbilityActorInfo(this,this);
	GASTPSASC->ApplyInitialEffects();
	GASTPSASC->GiveInitialAbilities();
}

void ACharacterBase::ClientSideInit()
{
	GASTPSASC->InitAbilityActorInfo(this,this);
}

bool ACharacterBase::IsLocallyControlledByPlayer() const
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, TeamID)
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatusWidget();
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return GASTPSASC;
}

void ACharacterBase::BindGASChangeDelegates()
{
	if(GASTPSASC)
	{
		GASTPSASC->RegisterGameplayTagEvent(UAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &ACharacterBase::DeathTagUpdated);
	}
}

void ACharacterBase::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		// NEW - Award score to killer's team
		if (HasAuthority())
		{
			AGASTPSGameState* GS = GetWorld()->GetGameState<AGASTPSGameState>();
			AController* MyController = GetController();
			
			// Get who killed me from damage context
			if (GS && MyController && LastDamageInstigator && LastDamageInstigator != MyController)
			{
				IGenericTeamAgentInterface* KillerTeam = Cast<IGenericTeamAgentInterface>(LastDamageInstigator);
				IGenericTeamAgentInterface* VictimTeam = Cast<IGenericTeamAgentInterface>(MyController);
				
				// Only award if enemy kill
				if (KillerTeam && VictimTeam && KillerTeam->GetGenericTeamId() != VictimTeam->GetGenericTeamId())
				{
					GS->AddScore(KillerTeam->GetGenericTeamId().GetId());
				}
			}
		}

		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}


void ACharacterBase::ConfigureOverHeadStatusWidget()
{
	if (!OverHeadWidgetComponent) return;

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	UOverHeadStatsGauge* OverheadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatsGauge)
	{
		OverheadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);
		GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
		GetWorldTimerManager().SetTimer(HeadStatGaugeVisibilityUpdateTimerHandle, this, &ACharacterBase::UpdateHeadGaugeVisibility, HeadStatGaugeVisibilityCheckUpdateGap, true);
	}
}

void ACharacterBase::UpdateHeadGaugeVisibility()
{
	// Need to be careful with player index 0  in case of Listen servers.
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatGaugeVisibilityRangeSquared);
	}
}

void ACharacterBase::SetStatusGaugeEnabled(bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	if (bIsEnabled)
	{
		ConfigureOverHeadStatusWidget();
	}
	else
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

void ACharacterBase::DeathMontageFinished()
{
	SetRagdollEnabled(true);
}

void ACharacterBase::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ACharacterBase::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle,this,&ACharacterBase::DeathMontageFinished,MontageDuration + DeathMontageFinishTimeShift);
	}
}

void ACharacterBase::StartDeathSequence()
{
	OnDead();
	PlayDeathAnimation();
	SetStatusGaugeEnabled(false);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACharacterBase::Respawn()
{
	OnRespawn();
	SetRagdollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);

	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}
	
	if (GASTPSASC)
	{
		GASTPSASC->ApplyFullStatEffect();
	}
}

void ACharacterBase::OnDead()
{
}

void ACharacterBase::OnRespawn()
{
}

void ACharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACharacterBase::GetGenericTeamId() const
{
	return TeamID;
}
