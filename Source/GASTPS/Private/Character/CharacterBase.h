// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "CharacterBase.generated.h"

UCLASS()
class ACharacterBase : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//Only Called on Server
	virtual void PossessedBy(AController* NewController) override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/******************/
	/*Gameplay Ability*/
	/******************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	void BindGASChangeDelegates();
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class UGASTPSASC* GASTPSASC;

	UPROPERTY()
	class UGASTPSAttributeSet* AttributeSet;	

	/******************/
	/* User Interface */
	/******************/

	UPROPERTY(VisibleDefaultsOnly, Category = "GameplayAbility")
	class UWidgetComponent* OverHeadWidgetComponent;
	void ConfigureOverHeadStatusWidget();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityCheckUpdateGap = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityRangeSquared = 10000000.f;
	
	FTimerHandle HeadStatGaugeVisibilityUpdateTimerHandle;

	void UpdateHeadGaugeVisibility();
	void SetStatusGaugeEnabled(bool bIsEnabled);
	
	/*******************/
	/* Death & Respawn */
	/*******************/
	FTransform MeshRelativeTransform;
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathMontageFinishTimeShift = -0.6f;
	
	FTimerHandle DeathMontageTimerHandle;

	void DeathMontageFinished();
	void SetRagdollEnabled(bool bIsEnabled);

	void PlayDeathAnimation();
	void StartDeathSequence();
	void Respawn();

	virtual void OnDead();
	virtual void OnRespawn();

	/******************/
	/*      Team      */
	/******************/
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY()
	AController* LastDamageInstigator; // Track killer
	
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
