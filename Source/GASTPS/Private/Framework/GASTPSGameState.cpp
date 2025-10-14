// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Framework/GASTPSGameState.h"
#include "Net/UnrealNetwork.h"

void AGASTPSGameState::RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot)
{
	if (!HasAuthority() || IsSlotOccupied(DesiredSlot)) return;

	FPlayerSelection* PlayerSelectionPtr = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.IsForPlayer(RequestingPlayer);
	});

	if (PlayerSelectionPtr)
	{
		PlayerSelectionPtr->SetSlot(DesiredSlot);
	}
	else
	{
		PlayerSelectionArray.Add(FPlayerSelection(DesiredSlot, RequestingPlayer));
	}
	//only needed for listen-server
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}

void AGASTPSGameState::SetCharacterSelected(const APlayerState* RequestingPlayer,
	const UPDA_CharacterDefinition* SelectedDefinition)
{
	if (IsDefinitionSelected(SelectedDefinition)) return;

	FPlayerSelection* PlayerSelectionPtr = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.IsForPlayer(RequestingPlayer);
	});

	if (PlayerSelectionPtr)
	{
		PlayerSelectionPtr->SetCharacterDefinition(SelectedDefinition);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

bool AGASTPSGameState::IsSlotOccupied(uint8 SlotId) const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray )
	{
		if (PlayerSelection.GetSlot() == SlotId) return true;
	}
	return false;
}

bool AGASTPSGameState::IsDefinitionSelected(const UPDA_CharacterDefinition* Definition) const
{
	const FPlayerSelection* PlayerSelectionPtr = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.GetCharacterDefinition() == Definition;
	}
	);
	return PlayerSelectionPtr != nullptr;
}

void AGASTPSGameState::SetCharacterDeselected(const UPDA_CharacterDefinition* DefinitionToDeselect)
{
	if (!DefinitionToDeselect) return;

	FPlayerSelection* PlayerSelectionPtr = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.GetCharacterDefinition() == DefinitionToDeselect;
	});

	if (PlayerSelectionPtr)
	{
		PlayerSelectionPtr->SetCharacterDefinition(nullptr);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

const TArray<FPlayerSelection>& AGASTPSGameState::GetPlayerSelection() const
{
	return PlayerSelectionArray;
}

bool AGASTPSGameState::CanStartHeroSelection() const
{
	return PlayerSelectionArray.Num() == PlayerArray.Num();
}

bool AGASTPSGameState::CanStartMatch() const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray)
	{
		if (PlayerSelection.GetCharacterDefinition() == nullptr) return false;
	}
	return true;
}

void AGASTPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AGASTPSGameState, PlayerSelectionArray, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(AGASTPSGameState, AlphaScore);
	DOREPLIFETIME(AGASTPSGameState, BravoScore);
}

void AGASTPSGameState::OnRep_PlayerSelectionArray()
{
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}
