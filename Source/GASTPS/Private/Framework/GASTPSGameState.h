// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInfoTypes.h"
#include "GASTPSGameState.generated.h"

class UPDA_CharacterDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated, const TArray<FPlayerSelection>& /*NewPlayerSelection*/)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChanged, int32, AlphaScore, int32, BravoScore);

/**
 * 
 */
UCLASS()
class AGASTPSGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	void RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot);
	void SetCharacterSelected(const APlayerState* RequestingPlayer, const UPDA_CharacterDefinition* SelectedDefinition);
	bool IsSlotOccupied(uint8 SlotId) const;
	bool IsDefinitionSelected(const UPDA_CharacterDefinition* Definition) const;
	void SetCharacterDeselected(const UPDA_CharacterDefinition* DefinitionToDeselect);
	
	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	const TArray<FPlayerSelection>& GetPlayerSelection() const;
	bool CanStartHeroSelection() const;
	bool CanStartMatch() const;

	
	UFUNCTION(BlueprintCallable)
	void AddScore(int32 TeamID)
	{
		if (HasAuthority()) { 
		TeamID == 0 ? ++AlphaScore : ++BravoScore;
			OnScoreChanged.Broadcast(AlphaScore, BravoScore); }
	}
	
	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnScoreChanged;

	UFUNCTION(BlueprintPure)
	int32 GetAlphaScore() const { return AlphaScore; }

	UFUNCTION(BlueprintPure)
	int32 GetBravoScore() const { return BravoScore; }
	
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;

	UFUNCTION()
	void OnRep_PlayerSelectionArray();
	
	UPROPERTY(ReplicatedUsing = OnRep_Scores)
	int32 AlphaScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Scores)
	int32 BravoScore = 0;
	
	UFUNCTION()
	void OnRep_Scores()
	{
		OnScoreChanged.Broadcast(AlphaScore, BravoScore);
	}
};