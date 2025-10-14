// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "GASTPSGameSession.generated.h"

UCLASS()
class GASTPS_API AGASTPSGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:	
	virtual bool ProcessAutoLogin() override;
	
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;
	virtual void UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId) override;
};
