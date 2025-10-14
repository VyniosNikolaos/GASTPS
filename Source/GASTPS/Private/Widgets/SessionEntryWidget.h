// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntryWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionEntrySelected, const FString& /*SelectedSessionIdStr*/)
/**
 * 
 */
UCLASS()
class GASTPS_API USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public:	
	virtual void NativeConstruct() override;
	FOnSessionEntrySelected OnSessionEntrySelected;
	void InitializeEntry(const FString& Name, const FString& SessionIdStr);
	FORCEINLINE FString GetCachedSessionIdStr() const { return CachedSessionIdStr; }
private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SessionNameText;

	UPROPERTY(meta=(BindWidget))
	class UButton* SessionBtn;

	FString CachedSessionIdStr;

	UFUNCTION()
	void SessionEntrySelected();
};

