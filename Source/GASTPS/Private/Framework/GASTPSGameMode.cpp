// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Framework/GASTPSGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Network/GASTPSGameSession.h"
#include "Player/GASTPSPlayerState.h"

AGASTPSGameMode::AGASTPSGameMode()
{
	GameSessionClass = AGASTPSGameSession::StaticClass();
	bUseSeamlessTravel = true;
}

APlayerController* AGASTPSGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	FGenericTeamId TeamId = GetTeamIDForPlayer(NewPlayerController);
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamId);
	
	return NewPlayerController;
}

void AGASTPSGameMode::StartPlay()
{
	Super::StartPlay();
}

UClass* AGASTPSGameMode::GetDefaultPawnClassForController_Implementation(AController* Controller)
{
	AGASTPSPlayerState* PlayerState = Controller->GetPlayerState<AGASTPSPlayerState>();
	UE_LOG(LogTemp, Warning, TEXT("[GetDefaultPawnClassForController] Controller: %s, PlayerState: %s"),
		*GetNameSafe(Controller), *GetNameSafe(PlayerState))

	if (PlayerState)
	{
		TSubclassOf<APawn> SelectedClass = PlayerState->GetSelectedPawnClass();
		UE_LOG(LogTemp, Warning, TEXT("[GetDefaultPawnClassForController] Selected Pawn Class: %s"),
			*GetNameSafe(SelectedClass))

		if (SelectedClass)
		{
			return SelectedClass;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[GetDefaultPawnClassForController] Returning BackupPawn: %s"),
		*GetNameSafe(BackupPawn))
	return BackupPawn;
}

APawn* AGASTPSGameMode::SpawnDefaultPawnFor_Implementation(AController* NewController, AActor* StartSpot)
{
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewController);
	FGenericTeamId TeamId = GetTeamIDForPlayer(NewController);
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}
	NewController->StartSpot = FindNextStartSpotForTeam(TeamId);
	//NewController->StartSpot = StartSpot;

	return Super::SpawnDefaultPawnFor_Implementation(NewController, StartSpot);
}

FGenericTeamId AGASTPSGameMode::GetTeamIDForPlayer(const AController* Controller) const
{
	AGASTPSPlayerState* PlayerState = Controller->GetPlayerState<AGASTPSPlayerState>();
	if (PlayerState && PlayerState->GetSelectedPawnClass())
	{
		return PlayerState->GetTeamIdBasedOnSlot();
	}
	
	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* AGASTPSGameMode::FindNextStartSpotForTeam(FGenericTeamId TeamID) const
{
	const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);
	if (!StartSpotTag)
	{
		return nullptr;
	}
	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		if (It->PlayerStartTag == *StartSpotTag)
		{
			It->PlayerStartTag = FName("Taken");
			return *It;
		}
	}
	return nullptr;
}

void AGASTPSGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	// After seamless travel, spawn and possess a new pawn for the player
	APlayerController* PC = Cast<APlayerController>(C);
	if (PC && !PC->GetPawn())
	{
		RestartPlayer(PC);
	}
}
