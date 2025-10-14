// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

/**
 * Simple crosshair widget that displays at the center of the screen
 */
UCLASS()
class GASTPS_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Show/hide the crosshair
	void ShowCrosshair();
	void HideCrosshair();

	// Update crosshair appearance based on aim state
	UFUNCTION(BlueprintCallable, Category = "Crosshair")
	void SetAimingState(bool bIsAiming);

private:
	UPROPERTY(meta=(BindWidget))
	class UImage* CrosshairImage;

	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	float NormalOpacity = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	float AimingOpacity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	FVector2D NormalSize = FVector2D(32.0f, 32.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	FVector2D AimingSize = FVector2D(24.0f, 24.0f);
};
