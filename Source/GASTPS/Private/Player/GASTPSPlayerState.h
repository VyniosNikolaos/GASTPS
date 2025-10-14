// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "GASTPSPlayerState.generated.h"

class UPDA_CharacterDefinition;

/**
 * 
 */
UCLASS()
class AGASTPSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AGASTPSPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	TSubclassOf<APawn> GetSelectedPawnClass() const;
	FGenericTeamId GetTeamIdBasedOnSlot() const;
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedCharacterDefinition(const UPDA_CharacterDefinition* NewCharacterDefinition);

private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelection;
	
	UPROPERTY()
	class AGASTPSGameState* GameState;

	void PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelection);
};
