// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "CharacterDisplay.h"
#include "Camera/CameraComponent.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ACharacterDisplay::ACharacterDisplay()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root Component"));

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh Component");
	MeshComponent->SetupAttachment(GetRootComponent());
	
	ViewCameraComponent = CreateDefaultSubobject<UCameraComponent>("View Camera Component");
	ViewCameraComponent->SetupAttachment(GetRootComponent());
}

void ACharacterDisplay::ConfigureWithCharacterDefinition(const UPDA_CharacterDefinition* CharacterDefinition)
{
	if (!CharacterDefinition)
		return;

	MeshComponent->SetSkeletalMesh(CharacterDefinition->LoadDisplayMesh());
	MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	MeshComponent->SetAnimClass(CharacterDefinition->LoadDisplayAnimationBP());
}