// Vynios Nikolaos , 2025, University of Patras, Thesis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASTPS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	virtual void NativeConstruct() override;
	
	/**************/	
	/*    Main    */
	/**************/		
private:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY()
	class UGASTPSGameInstance* GameInstance;

	void SwitchToMainWidget();

	UPROPERTY(meta=(BindWidget))
	class UWidget* MainWidgetRoot;

	/****************/	
	/*   Session    */
	/****************/		
	UPROPERTY(meta=(BindWidget))
	class UButton* CreateSessionBtn;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* NewSessionNameText;

	UFUNCTION()
	void CreateSessionBtnClicked();

	UFUNCTION()
	void CancelSessionCreation();

	UFUNCTION()
	void NewSessionNameTextChanged(const FText& NewText);

	void JoinSessionFailed();

	void UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults);

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* SessionScrollBox;

	UPROPERTY(meta=(BindWidget))
	class UButton* JoinSessionBtn;

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	TSubclassOf<class USessionEntryWidget> SessionEntryWidgetClass;

	FString CurrentSelectedSessionId = "";

	UFUNCTION()
	void JoinSessionBtnClicked();

	void SessionEntrySelected(const FString& SelectedEntryIdStr);

	/****************/	
	/*    Login     */
	/****************/	
private:
	UPROPERTY(meta=(BindWidget))
	class UWidget* LoginWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	class UButton* LoginBtn;

	UFUNCTION()
	void LoginBtnClicked();

	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg);

	/****************/	
	/*   Waiting    */
	/****************/		
private:
	UPROPERTY(meta=(BindWidget))
	class UWaitingWidget* WaitingWidget;
	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel = false);
};