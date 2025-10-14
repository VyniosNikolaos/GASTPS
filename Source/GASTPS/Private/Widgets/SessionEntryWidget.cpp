// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "SessionEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void USessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SessionBtn->OnClicked.AddDynamic(this, &USessionEntryWidget::SessionEntrySelected);
}

void USessionEntryWidget::InitializeEntry(const FString& Name, const FString& SessionIdStr)
{
	SessionNameText->SetText(FText::FromString(Name));
	CachedSessionIdStr = SessionIdStr;
}

void USessionEntryWidget::SessionEntrySelected()
{
	OnSessionEntrySelected.Broadcast(CachedSessionIdStr);
}