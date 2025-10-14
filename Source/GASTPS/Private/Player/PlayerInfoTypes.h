// VyniosNikolaos, Thesis, University of Patras, 2025
#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.generated.h"

class APlayerState;
class UPDA_CharacterDefinition;
/**
 * 
 */
USTRUCT()
struct FPlayerSelection
{
	GENERATED_BODY()
	
public:
	FPlayerSelection();
	FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState);

	FORCEINLINE void SetSlot(uint8 NewSlot) { Slot = NewSlot; }
	FORCEINLINE uint8 GetSlot() const { return Slot; }
	FORCEINLINE const FUniqueNetIdRepl& GetPlayerUniqueId() const { return PlayerUniqueId; }
	FORCEINLINE const FString& GetPlayerNickName() const { return PlayerNickName; }
	FORCEINLINE const UPDA_CharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }
	FORCEINLINE void SetCharacterDefinition(const UPDA_CharacterDefinition* NewCharacterDefinition) { CharacterDefinition = NewCharacterDefinition; }
	
	bool IsForPlayer(const APlayerState* PlayerState) const;
	bool IsValid() const;

	static uint8 GetInvalidSlot();
	
private:
	UPROPERTY()
	uint8 Slot;

	UPROPERTY()
	FUniqueNetIdRepl PlayerUniqueId;

	UPROPERTY()
	FString PlayerNickName;

	UPROPERTY()
	const UPDA_CharacterDefinition* CharacterDefinition;
};
