// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAS/GASTPSGameplayAbilityTypes.h"
#include "PDA_CharacterDefinition.generated.h"

class ACharacterBase;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class UPDA_CharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetCharacterDefinitionAssetType();

	FString GetCharacterDisplayName() const { return CharacterName; }
	UTexture2D* LoadIcon() const;
	TSubclassOf<ACharacterBase> LoadCharacterClass() const;
	TSubclassOf<UAnimInstance> LoadDisplayAnimationBP() const;
	class USkeletalMesh* LoadDisplayMesh() const;
	const TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>>* GetAbilities() const;

private:	
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FString CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<ACharacterBase> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<UAnimInstance> DisplayAnimBP;
};