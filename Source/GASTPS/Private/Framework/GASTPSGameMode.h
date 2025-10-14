// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GenericTeamAgentInterface.h"
#include "GASTPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AGASTPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGASTPSGameMode();
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	virtual void StartPlay() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* Controller) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

private:
	FGenericTeamId GetTeamIDForPlayer(const AController* Controller) const;

	AActor* FindNextStartSpotForTeam(FGenericTeamId TeamID) const;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TSubclassOf<APawn> BackupPawn;
	
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TMap<FGenericTeamId, FName> TeamStartSpotTagMap;

	//void MatchFinished(AActor* ViewTarget, int WinningTeam);
};
