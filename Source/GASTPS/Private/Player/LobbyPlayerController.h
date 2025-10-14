// VyniosNikolaos, Thesis, University of Patras, 2025
#pragma once

#include "CoreMinimal.h"
#include "Player/MenuPlayerController.h"
#include "LobbyPlayerController.generated.h"

DECLARE_DELEGATE(FOnSwitchToHeroSelection);

/**
 * 
 */
UCLASS()
class ALobbyPlayerController : public AMenuPlayerController
{
	GENERATED_BODY()

public:
	ALobbyPlayerController();
	FOnSwitchToHeroSelection OnSwitchToHeroSelection;

	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSlotSelectionChange(uint8 NewSlotID);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartHeroSelection();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartMatch();

	UFUNCTION(Client, Reliable)
	void Client_StartHeroSelection();
};
