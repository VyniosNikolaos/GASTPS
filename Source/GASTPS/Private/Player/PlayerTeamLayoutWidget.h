// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "PlayerTeamLayoutWidget.generated.h"

class UPlayerTeamSlotWidget;
/**
 * 
 */
UCLASS()
class GASTPS_API UPlayerTeamLayoutWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	void UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections);
private:	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float PlayerTeamWidgetSlotMargin = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UPlayerTeamSlotWidget> PlayerTeamSlotWidgetClass;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* TeamOneLayoutBox;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* TeamTwoLayoutBox;

	UPROPERTY()
	TArray<UPlayerTeamSlotWidget*> TeamSlotWidgets;
};