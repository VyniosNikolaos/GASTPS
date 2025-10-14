// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UPDA_CharacterDefinition;

/**
 * 
 */
UCLASS()
class GASTPS_API UPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	void UpdateSlot(const FString& PlayerName, const UPDA_CharacterDefinition* CharacterDefinition);

	virtual void NativeOnMouseEnter( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent ) override;

private:	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	class UWidgetAnimation* HoverAnim;

	UPROPERTY(meta=(BindWidget))
	class UImage* PlayerCharacterIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterIconMatParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterEmptyMatParamName = "Empty";

	FString CachedPlayerNameStr;
	FString CachedCharacterNameStr;

	void UpdateNameText();
};
