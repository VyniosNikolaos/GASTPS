// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamScoreWidget.generated.h"

/**
 * Simple widget that displays team scores
 */
UCLASS()
class GASTPS_API UTeamScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AlphaScore;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BravoScore;

private:
	UFUNCTION()
	void UpdateScores(int32 Alpha, int32 Bravo);
};