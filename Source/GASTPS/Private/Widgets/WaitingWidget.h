// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WaitingWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASTPS_API UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	virtual void NativeConstruct() override;

	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent();
	void SetWaitInfo(const FText& WaitInfo, bool bAllowCancel = false);

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* WaitInfoText;

	UPROPERTY(meta=(BindWidget))
	UButton* CancelBtn;
};