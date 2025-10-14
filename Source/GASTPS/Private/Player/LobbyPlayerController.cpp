// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Player/LobbyPlayerController.h"

#include "Framework/GASTPSGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Framework/GASTPSGameState.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void ALobbyPlayerController::Server_RequestStartMatch_Implementation()
{
	UGASTPSGameInstance* GameInstance = GetGameInstance<UGASTPSGameInstance>();
	if (GameInstance)
	{
		GameInstance->StartMatch();
	}
}

bool ALobbyPlayerController::Server_RequestStartMatch_Validate()
{
	return true;
}


void ALobbyPlayerController::Server_RequestSlotSelectionChange_Implementation(uint8 NewSlotID)
{
	if (!GetWorld()) return;

	AGASTPSGameState* GameState = GetWorld()->GetGameState<AGASTPSGameState>();
	if (!GameState) return;

	GameState->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotID);
}

bool ALobbyPlayerController::Server_RequestSlotSelectionChange_Validate(uint8 NewSlotID)
{
	return true;
}

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld())
		return;

	for (FConstPlayerControllerIterator PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(*PlayerControllerIterator);
		if (PlayerController)
		{
			PlayerController->Client_StartHeroSelection();
		}
	}
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	OnSwitchToHeroSelection.ExecuteIfBound();
}

