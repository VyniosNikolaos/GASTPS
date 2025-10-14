// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterEntryWidget.generated.h"

class UPDA_CharacterDefinition;
/**
 * 
 */
UCLASS()
class GASTPS_API UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	UFUNCTION(BlueprintCallable, Category = "Character Entry")
	void SetCharacterData(UPDA_CharacterDefinition* InCharacterDefinition);
	
	FORCEINLINE const UPDA_CharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }
	void SetSelected(bool bIsSelected);
	
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* CharacterImage;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName IconTextureMatParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName SaturationMatParamName = "Saturation";

	UPROPERTY()
	const UPDA_CharacterDefinition* CharacterDefinition;
};