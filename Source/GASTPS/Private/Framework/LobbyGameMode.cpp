// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Framework/LobbyGameMode.h"
#include "Network/GASTPSGameSession.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = AGASTPSGameSession::StaticClass();

}
