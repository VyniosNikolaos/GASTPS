// VyniosNikolaos, Thesis, University of Patras, 2025

#include "LobbyWidget.h"
#include "CharacterDisplay.h"
#include "CharacterEntryWidget.h"
#include "Components/Button.h"
#include "TeamSelectionWidget.h"
#include "Components/UniformGridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/GASTPSGameState.h"
#include "Network/GASTPSNetStatics.h"
#include "Player/LobbyPlayerController.h"
#include "Character/GASTPSAssetManager.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Components/TileView.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GASTPSPlayerState.h"
#include "Player/PlayerTeamLayoutWidget.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearAndPopulateTeamSelectionSlots();
	ConfigureGameState();
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	if (LobbyPlayerController)
	{
		LobbyPlayerController->OnSwitchToHeroSelection.BindUObject(this, &ULobbyWidget::SwitchToHeroSelection);
	}

	StartHeroSelectionButton->SetIsEnabled(false);
	StartHeroSelectionButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartHeroSelectionButtonClicked);
	StartMatchButton->SetIsEnabled(false);
	StartMatchButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartMatchButtonClicked);
	
	UGASTPSAssetManager::Get().LoadCharacterDefinition(FStreamableDelegate::CreateUObject(this, &ULobbyWidget::CharacterDefinitionLoaded));
	if (CharacterSelectionTileView)
	{
		CharacterSelectionTileView->OnItemSelectionChanged().AddUObject(this, &ULobbyWidget::CharacterSelected);
	}
	
	SpawnCharacterDisplay();
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();

	for (int i = 0; i < UGASTPSNetStatics::GetPlayerCountPerTeam() * 2; ++i)
	{
		UTeamSelectionWidget* NewSelectionSlot = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass);
		if (NewSelectionSlot)
		{
			NewSelectionSlot->SetSlotID(i);
			UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot);
			if (NewGridSlot)
			{
				int Row = i % UGASTPSNetStatics::GetPlayerCountPerTeam();
				int Column = i < UGASTPSNetStatics::GetPlayerCountPerTeam() ? 0 : 1;
				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Column);
			}

			NewSelectionSlot->OnSlotClicked.AddUObject(this, &ULobbyWidget::SlotSelected);
			TeamSelectionSlots.Add(NewSelectionSlot);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotID)
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_RequestSlotSelectionChange(NewSlotID);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	UWorld* World = GetWorld();
	if (!World) return;
	GameState = World->GetGameState<AGASTPSGameState>();
	if (!GameState)
	{
		World->GetTimerManager().SetTimer(ConfigureGameStateTimerHandle, this, &ULobbyWidget::ConfigureGameState, 1.f, true);
	}
	else
	{
		GameState->OnPlayerSelectionUpdated.AddUObject(this, &ULobbyWidget::UpdatePlayerSelectionDisplay);
		UpdatePlayerSelectionDisplay(GameState->GetPlayerSelection());
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UTeamSelectionWidget* SelectionSlot : TeamSelectionSlots)
	{
		SelectionSlot->UpdateSlotInfo("Empty");
	}

	for (UUserWidget* CharacterEntry : CharacterSelectionTileView->GetDisplayedEntryWidgets())
	{
		if (UCharacterEntryWidget* CharacterEntryWidget = Cast<UCharacterEntryWidget>(CharacterEntry))
		{
			CharacterEntryWidget->SetSelected(false);
		}
	}
	
	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid()) continue;
	
		TeamSelectionSlots[PlayerSelection.GetSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());

		UCharacterEntryWidget* SelectedEntry = CharacterSelectionTileView->GetEntryWidgetFromItem
		<UCharacterEntryWidget>(PlayerSelection.GetCharacterDefinition());
		if (SelectedEntry)
		{
			SelectedEntry->SetSelected(true);
		}

		if (PlayerSelection.IsForPlayer(GetOwningPlayerState()))
		{
			UpdateCharacterDisplay(PlayerSelection);
		}
	}
	
	if (GameState)
	{
		StartHeroSelectionButton->SetIsEnabled(GameState->CanStartHeroSelection());
		StartMatchButton->SetIsEnabled(GameState->CanStartMatch());
	}
	
	if (PlayerTeamLayoutWidget)
	{
		PlayerTeamLayoutWidget->UpdatePlayerSelection(PlayerSelections);
	}
}

void ULobbyWidget::StartHeroSelectionButtonClicked()
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelection()
{
	UE_LOG(LogTemp, Warning, TEXT("SwitchToHeroSelection called. TileView items=%d"), 
		CharacterSelectionTileView ? CharacterSelectionTileView->GetNumItems() : -1);
	
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::CharacterDefinitionLoaded()
{
	TArray<UPDA_CharacterDefinition*> LoadedCharacterDefinitions;
	if (UGASTPSAssetManager::Get().GetLoadedCharacterDefinition(LoadedCharacterDefinitions))
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterDefinitionLoaded: %d characters loaded"), LoadedCharacterDefinitions.Num());
		
		for (int i = 0; i < LoadedCharacterDefinitions.Num(); i++)
		{
			if (LoadedCharacterDefinitions[i])
			{
				UE_LOG(LogTemp, Warning, TEXT("  [%d] Adding to TileView: %s (Ptr: %p)"), i, *LoadedCharacterDefinitions[i]->GetCharacterDisplayName(), LoadedCharacterDefinitions[i]);
			}
		}
		
		if (CharacterSelectionTileView)
		{
			CharacterSelectionTileView->SetListItems(LoadedCharacterDefinitions);
			UE_LOG(LogTemp, Warning, TEXT("TileView SetListItems called. TileView item count: %d"), CharacterSelectionTileView->GetNumItems());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CharacterSelectionTileView is NULL!"));
		}
	}
}

void ULobbyWidget::CharacterSelected(UObject* SelectedUObject)
{
	if (!PlayerState)
	{
		PlayerState = GetOwningPlayerState<AGASTPSPlayerState>();
	}

	if (!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is NULL!"));
		return;
	}

	if (const UPDA_CharacterDefinition* SelectedCharacterDefinition = Cast<UPDA_CharacterDefinition>(SelectedUObject))
	{
		PlayerState->Server_SetSelectedCharacterDefinition(SelectedCharacterDefinition);
	}
}

void ULobbyWidget::SpawnCharacterDisplay()
{
	if (CharacterDisplay) return;
	if (!CharacterDisplayClass) return;

	FTransform DisplayTransform = FTransform::Identity;
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart)
	{
		DisplayTransform = PlayerStart->GetActorTransform();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharacterDisplayClass, DisplayTransform, SpawnParams);
	GetOwningPlayer()->SetViewTarget(CharacterDisplay);
}

void ULobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection)
{
	if (!PlayerSelection.GetCharacterDefinition())
		return;

	CharacterDisplay->ConfigureWithCharacterDefinition(PlayerSelection.GetCharacterDefinition());

}

void ULobbyWidget::StartMatchButtonClicked()
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_RequestStartMatch();
	}
}
