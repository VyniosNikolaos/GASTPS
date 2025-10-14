// VyniosNikolaos, Thesis, University of Patras, 2025


#include "GASTPSGameSession.h"
#include "Framework/GASTPSGameInstance.h"


bool AGASTPSGameSession::ProcessAutoLogin()
{
	//return Super::ProcessAutoLogin();
	return true;
}

void AGASTPSGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId,
	bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	if (UGASTPSGameInstance* GameInst = GetGameInstance<UGASTPSGameInstance>())
	{
		GameInst->PlayerJoined(UniqueId);
	}
}

void AGASTPSGameSession::UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(FromSessionName, UniqueId);
	if (UGASTPSGameInstance* GameInst = GetGameInstance<UGASTPSGameInstance>())
	{
		GameInst->PlayerLeft(UniqueId);
	}
}
