// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Player/GASTPSPlayerState.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/GASTPSGameState.h"
#include "Net/UnrealNetwork.h"
#include "Network/GASTPSNetStatics.h"
#include "Character/CharacterBase.h"

AGASTPSPlayerState::AGASTPSPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.0f;
}

void AGASTPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGASTPSPlayerState, PlayerSelection)
}

void AGASTPSPlayerState::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<AGASTPSGameState>(UGameplayStatics::GetGameState(this));

	if (GameState)
	{
		GameState->OnPlayerSelectionUpdated.AddUObject(this, &AGASTPSPlayerState::PlayerSelectionUpdated);
	}
}

void AGASTPSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("[CopyProperties] Copying from OLD PlayerState %s to NEW PlayerState %s"),
		*GetNameSafe(this), *GetNameSafe(PlayerState))

	AGASTPSPlayerState* NewPlayerState = Cast<AGASTPSPlayerState>(PlayerState);
	if (NewPlayerState)
	{
		// Copy FROM this (old PlayerState) TO the new PlayerState
		NewPlayerState->PlayerSelection = PlayerSelection;

		UE_LOG(LogTemp, Warning, TEXT("[CopyProperties] Copied CharacterDefinition: %s, Slot: %d"),
			*GetNameSafe(PlayerSelection.GetCharacterDefinition()), PlayerSelection.GetSlot())
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[CopyProperties] Failed to cast new PlayerState!"))
	}
}

TSubclassOf<APawn> AGASTPSPlayerState::GetSelectedPawnClass() const
{
	const UPDA_CharacterDefinition* CharDef = PlayerSelection.GetCharacterDefinition();
	UE_LOG(LogTemp, Warning, TEXT("[GetSelectedPawnClass] PlayerState: %s, CharacterDefinition: %s"),
		*GetNameSafe(this), *GetNameSafe(CharDef))

	if (CharDef)
	{
		TSubclassOf<APawn> PawnClass = CharDef->LoadCharacterClass();
		UE_LOG(LogTemp, Warning, TEXT("[GetSelectedPawnClass] Returning Pawn Class: %s"), *GetNameSafe(PawnClass))
		return PawnClass;
	}

	UE_LOG(LogTemp, Warning, TEXT("[GetSelectedPawnClass] No character definition, returning nullptr"))
	return nullptr;
}

FGenericTeamId AGASTPSPlayerState::GetTeamIdBasedOnSlot() const
{
	return PlayerSelection.GetSlot() < UGASTPSNetStatics::GetPlayerCountPerTeam() ? FGenericTeamId{ 0 } : FGenericTeamId{ 1 };
}

void AGASTPSPlayerState::Server_SetSelectedCharacterDefinition_Implementation(
	const UPDA_CharacterDefinition* NewCharacterDefinition)
{
	if (!GameState) return;
	if (!NewCharacterDefinition) return;

	if (GameState->IsDefinitionSelected(NewCharacterDefinition)) return;
	if (PlayerSelection.GetCharacterDefinition())
	{
		GameState->SetCharacterDeselected(PlayerSelection.GetCharacterDefinition());
	}

	PlayerSelection.SetCharacterDefinition(NewCharacterDefinition);
	GameState->SetCharacterSelected(this, NewCharacterDefinition);
}

bool AGASTPSPlayerState::Server_SetSelectedCharacterDefinition_Validate(
	const UPDA_CharacterDefinition* NewCharacterDefinition)
{
	return true;
}

void AGASTPSPlayerState::PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelection)
{
	for (const FPlayerSelection& Selection : NewPlayerSelection)
	{
		if (Selection.IsForPlayer(this))
		{
			PlayerSelection = Selection;
		}
	}
}
