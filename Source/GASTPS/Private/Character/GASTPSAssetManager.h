// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GASTPSAssetManager.generated.h"

class UPDA_CharacterDefinition;
/**
 * 
 */
UCLASS()
class GASTPS_API UGASTPSAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UGASTPSAssetManager& Get();
	void LoadCharacterDefinition(const FStreamableDelegate& LoadFinishedCallback);
	bool GetLoadedCharacterDefinition(TArray<UPDA_CharacterDefinition*>& LoadedCharacterDefinitions) const;
};
