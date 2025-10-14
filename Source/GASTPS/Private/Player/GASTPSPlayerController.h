// VyniosNikolaos, Thesis, University of Patras, 2025
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "GASTPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AGASTPSPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AGASTPSPlayerController();
	//only called on server by engine design
	virtual void OnPossess(APawn* NewPawn) override;
	// only called on the client, also on the listen server
	virtual void AcknowledgePossession(APawn* P) override;
	// called after seamless travel to reset player state
	virtual void PostSeamlessTravel() override;

	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SpawnGameplayWidget();
	
	UPROPERTY()
	class AGASTPSPlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	class UGameplayWidget* GameplayWidget;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
