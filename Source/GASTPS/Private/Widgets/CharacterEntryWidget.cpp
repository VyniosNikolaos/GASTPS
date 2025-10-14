// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "CharacterEntryWidget.h"

#include "Character/PDA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UE_LOG(LogTemp, Warning, TEXT("NativeOnListItemObjectSet called with object: %s"), ListItemObject ? *ListItemObject->GetName() : TEXT("NULL"));
	
	SetCharacterData(Cast<UPDA_CharacterDefinition>(ListItemObject));
}

void UCharacterEntryWidget::SetCharacterData(UPDA_CharacterDefinition* InCharacterDefinition)
{
	CharacterDefinition = InCharacterDefinition;
	
	if (CharacterDefinition)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterEntryWidget: Setting character to %s, CharacterImage=%s, CharacterName=%s"), 
			*CharacterDefinition->GetCharacterDisplayName(),
			CharacterImage ? TEXT("Valid") : TEXT("NULL"),
			CharacterName ? TEXT("Valid") : TEXT("NULL"));
		
		if (CharacterImage && CharacterName)
		{
			UMaterialInstanceDynamic* DynMat = CharacterImage->GetDynamicMaterial();
			UE_LOG(LogTemp, Warning, TEXT("  -> GetDynamicMaterial returned: %s"), DynMat ? TEXT("Valid") : TEXT("NULL"));
			
			if (DynMat)
			{
				UTexture2D* Icon = CharacterDefinition->LoadIcon();
				UE_LOG(LogTemp, Warning, TEXT("  -> LoadIcon returned: %s"), Icon ? *Icon->GetName() : TEXT("NULL"));
				DynMat->SetTextureParameterValue(IconTextureMatParamName, Icon);
			}
			
			FText NewText = FText::FromString(CharacterDefinition->GetCharacterDisplayName());
			CharacterName->SetText(NewText);
			UE_LOG(LogTemp, Warning, TEXT("  -> SetText called with: %s, current text is: %s"), 
				*NewText.ToString(), 
				*CharacterName->GetText().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CharacterEntryWidget: CharacterImage or CharacterName is NULL!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterEntryWidget: CharacterDefinition is NULL!"));
	}
}

void UCharacterEntryWidget::SetSelected(bool bIsSelected)
{
	if (UMaterialInstanceDynamic* DynMat = CharacterImage->GetDynamicMaterial())
	{
		DynMat->SetScalarParameterValue(SaturationMatParamName, bIsSelected ? 0.f : 1.f);
	}
}
