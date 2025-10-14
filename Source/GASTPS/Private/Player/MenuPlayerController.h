// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Menu")
	TSubclassOf<class UUserWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Menu")
	 UUserWidget* MenuWidget;

	void SpawnWidget();
};
