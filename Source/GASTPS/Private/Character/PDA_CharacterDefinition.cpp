// VyniosNikolaos, Thesis, University of Patras, 2025

#include "PDA_CharacterDefinition.h"
#include "Character/CharacterBase.h"

FPrimaryAssetId UPDA_CharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinitionAssetType(), GetFName());
}

FPrimaryAssetType UPDA_CharacterDefinition::GetCharacterDefinitionAssetType()
{
	return FPrimaryAssetType(TEXT("CharacterDefinition"));
}

UTexture2D* UPDA_CharacterDefinition::LoadIcon() const
{
	CharacterIcon.LoadSynchronous();
	if (CharacterIcon.IsValid())
		return CharacterIcon.Get();

	return nullptr;
}

TSubclassOf<ACharacterBase> UPDA_CharacterDefinition::LoadCharacterClass() const
{
	CharacterClass.LoadSynchronous();
	if (CharacterClass.IsValid())
		return CharacterClass.Get();
	
	return TSubclassOf<ACharacterBase>();

}

TSubclassOf<UAnimInstance> UPDA_CharacterDefinition::LoadDisplayAnimationBP() const
{
	DisplayAnimBP.LoadSynchronous();
	if (DisplayAnimBP.IsValid())
		return DisplayAnimBP.Get();

	return TSubclassOf<UAnimInstance>();
}

USkeletalMesh* UPDA_CharacterDefinition::LoadDisplayMesh() const
{
	TSubclassOf<ACharacterBase> LoadedCharacterClass = LoadCharacterClass();
	if (!LoadedCharacterClass)
		return nullptr;

	ACharacter* Character = Cast<ACharacter>(LoadedCharacterClass.GetDefaultObject());
	if (!Character)
		return nullptr;

	return Character->GetMesh()->GetSkeletalMeshAsset();
}

const TMap<EAbilityInputID, TSubclassOf<UGameplayAbility>>* UPDA_CharacterDefinition::GetAbilities() const
{
	TSubclassOf<ACharacterBase> LoadedCharacterClass = LoadCharacterClass();
	if (!LoadedCharacterClass)
		return nullptr;

	ACharacterBase* Character = Cast<ACharacterBase>(LoadedCharacterClass.GetDefaultObject());
	if (!Character)
		return nullptr;

	return nullptr;
	//return &(Character->GetAbilities());
}
