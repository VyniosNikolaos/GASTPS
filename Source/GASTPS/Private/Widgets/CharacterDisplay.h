// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

class UPDA_CharacterDefinition;

UCLASS()
class GASTPS_API ACharacterDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterDisplay();
	void ConfigureWithCharacterDefinition(const UPDA_CharacterDefinition* CharacterDefinition);

private:	
	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	class UCameraComponent* ViewCameraComponent;

};