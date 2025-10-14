// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Player/PlayerInfoTypes.h"
#include "GameFramework/PlayerState.h"
#include "Network/GASTPSNetStatics.h"

FPlayerSelection::FPlayerSelection()
	:Slot { GetInvalidSlot()}, PlayerUniqueId{
		FUniqueNetIdRepl::Invalid()}, PlayerNickName{}, CharacterDefinition{nullptr}
{
}

FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState)
	:Slot{InSlot}, CharacterDefinition{nullptr}
{
	if (InPlayerState)
	{
		PlayerUniqueId = InPlayerState->GetUniqueId();
		PlayerNickName = InPlayerState->GetPlayerName();
	}
}

bool FPlayerSelection::IsForPlayer(const APlayerState* PlayerState) const
{
	if (!PlayerState) return false;

#if WITH_EDITOR
	return PlayerState->GetPlayerName() == PlayerNickName;
#else
	return PlayerState->GetUniqueId() == GetPlayerUniqueId();
#endif
}

bool FPlayerSelection::IsValid() const
{
#if WITH_EDITOR
	return true;
#else
	if (!PlayerUniqueId.IsValid()) return false;
	if (Slot == GetInvalidSlot()) return false;
	if (Slot >= UGASTPSNetStatics::GetPlayerCountPerTeam()*2) return false;
	return true;
#endif
}

uint8 FPlayerSelection::GetInvalidSlot()
{
	return uint8(255);
}
