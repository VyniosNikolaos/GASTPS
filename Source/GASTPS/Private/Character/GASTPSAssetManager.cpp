// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "GASTPSAssetManager.h"

#include "PDA_CharacterDefinition.h"


void UGASTPSAssetManager::LoadCharacterDefinition(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UPDA_CharacterDefinition::GetCharacterDefinitionAssetType(), TArray<FName>(), LoadFinishedCallback);
}

bool UGASTPSAssetManager::GetLoadedCharacterDefinition(TArray<UPDA_CharacterDefinition*>& LoadedCharacterDefinitions) const
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UPDA_CharacterDefinition::GetCharacterDefinitionAssetType(), LoadedObjects);
	if (bLoaded)
	{
		for (UObject* LoadedObject : LoadedObjects)
		{
			LoadedCharacterDefinitions.Add(Cast<UPDA_CharacterDefinition>(LoadedObject));
		}
	}

	return bLoaded;
}

UGASTPSAssetManager& UGASTPSAssetManager::Get()
{
	UGASTPSAssetManager* Singleton = Cast<UGASTPSAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}

	return (*NewObject<UGASTPSAssetManager>());
}

