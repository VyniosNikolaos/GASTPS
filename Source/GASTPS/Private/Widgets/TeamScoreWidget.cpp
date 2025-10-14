#include "Widgets/TeamScoreWidget.h"
#include "Components/TextBlock.h"
#include "Framework/GASTPSGameState.h"

void UTeamScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AGASTPSGameState* GS = GetWorld()->GetGameState<AGASTPSGameState>())
	{
		// Bind to future updates
		GS->OnScoreChanged.AddDynamic(this, &UTeamScoreWidget::UpdateScores);
		
		// Initialize with current scores (in case widget was created late)
		UpdateScores(GS->GetAlphaScore(), GS->GetBravoScore());
	}
}

void UTeamScoreWidget::UpdateScores(int32 Alpha, int32 Bravo)
{
	if (AlphaScore) AlphaScore->SetText(FText::AsNumber(Alpha));
	if (BravoScore) BravoScore->SetText(FText::AsNumber(Bravo));
}