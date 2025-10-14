// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "PlayerTeamLayoutWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Network/GASTPSNetStatics.h"
#include "Widgets/PlayerTeamSlotWidget.h"

void UPlayerTeamLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	TeamOneLayoutBox->ClearChildren();
	TeamTwoLayoutBox->ClearChildren();

	if (!PlayerTeamSlotWidgetClass) return;

	for (int i = 0; i < UGASTPSNetStatics::GetPlayerCountPerTeam() * 2; ++i)
	{
		UPlayerTeamSlotWidget* NewSlotWidget = CreateWidget<UPlayerTeamSlotWidget>(GetOwningPlayer(), PlayerTeamSlotWidgetClass);
		TeamSlotWidgets.Add(NewSlotWidget);
		
		UHorizontalBoxSlot* NewSlot;
		if (i < UGASTPSNetStatics::GetPlayerCountPerTeam())
		{
			NewSlot = TeamOneLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
		}
		else
		{
			NewSlot = TeamTwoLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
		}

		NewSlot->SetPadding(FMargin{ PlayerTeamWidgetSlotMargin });
	}
}

void UPlayerTeamLayoutWidget::UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UPlayerTeamSlotWidget* SlotWidget : TeamSlotWidgets)
	{
		SlotWidget->UpdateSlot("", nullptr);
	}

	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid())
			continue;

		TeamSlotWidgets[PlayerSelection.GetSlot()]->UpdateSlot(PlayerSelection.GetPlayerNickName(), PlayerSelection.GetCharacterDefinition());
	}
}
