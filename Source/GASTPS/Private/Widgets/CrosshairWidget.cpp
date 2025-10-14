// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "CrosshairWidget.h"
#include "Components/Image.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set initial state
	if (CrosshairImage)
	{
		CrosshairImage->SetOpacity(NormalOpacity);
	}
}

void UCrosshairWidget::ShowCrosshair()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCrosshairWidget::HideCrosshair()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UCrosshairWidget::SetAimingState(bool bIsAiming)
{
	if (!CrosshairImage) return;

	// Change opacity and size based on aim state
	float TargetOpacity = bIsAiming ? AimingOpacity : NormalOpacity;
	CrosshairImage->SetOpacity(TargetOpacity);

	
	 FVector2D TargetSize = bIsAiming ? AimingSize : NormalSize;
	 CrosshairImage->SetDesiredSizeOverride(TargetSize);
}
