// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Player/GASTPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Player/GASTPSPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/GameplayWidget.h"

AGASTPSPlayerController::AGASTPSPlayerController()
{
	bAutoManageActiveCameraTarget = true;
}

void AGASTPSPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PlayerCharacter = Cast<AGASTPSPlayerCharacter>(NewPawn);
	if (PlayerCharacter)
	{
		PlayerCharacter->ServerSideInit();
		PlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void AGASTPSPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	PlayerCharacter = Cast<AGASTPSPlayerCharacter>(P);
	if (PlayerCharacter)
	{
		PlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();

		// Reset input mode for gameplay
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
	}
}

void AGASTPSPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	// Reset input mode after seamless travel to ensure player can control character
	if (IsLocalPlayerController())
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
	}
}

void AGASTPSPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId AGASTPSPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void AGASTPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGASTPSPlayerController, TeamID)
}

void AGASTPSPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController()) return;

	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
	}
}
