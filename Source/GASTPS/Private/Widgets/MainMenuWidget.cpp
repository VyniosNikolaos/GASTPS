// Vynios Nikolaos , 2025, University of Patras, Thesis


#include "MainMenuWidget.h"
#include "SessionEntryWidget.h"
#include "WaitingWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/GASTPSGameInstance.h"
#include "Network/GASTPSNetStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstance = GetGameInstance<UGASTPSGameInstance>();
	if (GameInstance)
	{
		GameInstance->OnLoginCompleted.AddUObject(this, &UMainMenuWidget::LoginCompleted);
		if (GameInstance->IsLoggedIn())
		{
			SwitchToMainWidget();
		}

		GameInstance->OnJoinSessionFailed.AddUObject(this, &UMainMenuWidget::JoinSessionFailed);
		GameInstance->OnGlobalSessionSearchCompleted.AddUObject(this, &UMainMenuWidget::UpdateLobbyList);
		GameInstance->StartGlobalSessionSearch();

	}

	LoginBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::LoginBtnClicked);

	CreateSessionBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::CreateSessionBtnClicked);
	CreateSessionBtn->SetIsEnabled(false);

	NewSessionNameText->OnTextChanged.AddDynamic(this, &UMainMenuWidget::NewSessionNameTextChanged);
	JoinSessionBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::JoinSessionBtnClicked);
	JoinSessionBtn->SetIsEnabled(false);
}

void UMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}
}

void UMainMenuWidget::CreateSessionBtnClicked()
{
	if (GameInstance && GameInstance->IsLoggedIn())
	{
		GameInstance->RequestCreateAndJoinSession(FName(NewSessionNameText->GetText().ToString()));
		SwitchToWaitingWidget(FText::FromString("Creating Lobby"), true).AddDynamic(this, &UMainMenuWidget::CancelSessionCreation);
	}
}

void UMainMenuWidget::CancelSessionCreation()
{
	if (GameInstance)
	{
		GameInstance->CancelSessionCreation();
	}
	SwitchToMainWidget();
}

void UMainMenuWidget::NewSessionNameTextChanged(const FText& NewText)
{
	CreateSessionBtn->SetIsEnabled(!NewText.IsEmpty());
}

void UMainMenuWidget::JoinSessionFailed()
{
	SwitchToMainWidget();
}

void UMainMenuWidget::UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("Updating Session Search Results"))
	SessionScrollBox->ClearChildren();

	if (!SessionEntryWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SessionEntryWidgetClass is null! Please assign it in the MainMenuWidget Blueprint."))
		return;
	}

	bool bCurrentSelectedSessionValid = false;
	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
	{
		USessionEntryWidget* NewSessionWidget = CreateWidget<USessionEntryWidget>(GetOwningPlayer(), SessionEntryWidgetClass);
		if (NewSessionWidget)
		{
			FString SessionName = "Name_None";
			SearchResult.Session.SessionSettings.Get<FString>(UGASTPSNetStatics::GetSessionNameKey(), SessionName);

			FString SessionIdStr = SearchResult.Session.GetSessionIdStr();
			NewSessionWidget->InitializeEntry(SessionName, SessionIdStr);
			NewSessionWidget->OnSessionEntrySelected.AddUObject(this, &UMainMenuWidget::SessionEntrySelected);
			SessionScrollBox->AddChild(NewSessionWidget);
			if (CurrentSelectedSessionId == SessionIdStr)
			{
				bCurrentSelectedSessionValid = true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create SessionEntryWidget for session: %s"), *SearchResult.GetSessionIdStr())
		}
	}

	CurrentSelectedSessionId = bCurrentSelectedSessionValid ? CurrentSelectedSessionId : "";
	JoinSessionBtn->SetIsEnabled(bCurrentSelectedSessionValid);
}

void UMainMenuWidget::JoinSessionBtnClicked()
{
	if (GameInstance && !CurrentSelectedSessionId.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to join session with id: %s"), *CurrentSelectedSessionId)
		if (GameInstance->JoinSessionWithId(CurrentSelectedSessionId))
		{
			SwitchToWaitingWidget(FText::FromString("Joining"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't join session, no session selected"))
	}
}

void UMainMenuWidget::SessionEntrySelected(const FString& SelectedEntryIdStr)
{
	CurrentSelectedSessionId = SelectedEntryIdStr;
}

void UMainMenuWidget::LoginBtnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Logging In!"))
	if (GameInstance && !GameInstance->IsLoggedIn() && !GameInstance->IsLoggingIn())
	{
		GameInstance->ClientAccountPortalLogin();
		SwitchToWaitingWidget(FText::FromString("Logging In"));
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login successful"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Failed!"))
	}
		
	SwitchToMainWidget();
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel)
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->SetWaitInfo(WaitInfo, bAllowCancel);
	return WaitingWidget->ClearAndGetButtonClickedEvent();
}
