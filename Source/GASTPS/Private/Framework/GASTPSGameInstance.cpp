// VyniosNikolaos, Thesis, University of Patras, 2025

#include "GASTPSGameInstance.h"
#include "Network/GASTPSNetStatics.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "HttpModule.h"

void UGASTPSGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		LoadLevelAndListen(GameLevel);
	}
}

void UGASTPSGameInstance::Init()
{
	Super::Init();
	if (GetWorld()->IsEditorWorld()) return;

	if (UGASTPSNetStatics::IsSessionServer(this))
	{
		CreateSession();
	} 
}

bool UGASTPSGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr IdentityPtr = UGASTPSNetStatics::GetIdentityPtr(this))
	{
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}

	return false;
}

bool UGASTPSGameInstance::IsLoggingIn() const
{
	return LoggingInDelegateHandle.IsValid();
}

void UGASTPSGameInstance::ClientAccountPortalLogin()
{
	ClientLogin("AccountPortal", "", "");
}

void UGASTPSGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	if (IOnlineIdentityPtr IdentityPtr = UGASTPSNetStatics::GetIdentityPtr(this))
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UGASTPSGameInstance::LoginCompleted);
		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed Right Away!"))
			if (LoggingInDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
				LoggingInDelegateHandle.Reset();
			}
			OnLoginCompleted.Broadcast(false, "", "Login Failed Right Away!");
		}
	}
}

void UGASTPSGameInstance::LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	if (IOnlineIdentityPtr IdentityPtr = UGASTPSNetStatics::GetIdentityPtr(this))
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		FString PlayerNickname = "";
		if (bWasSuccessful)
		{
			PlayerNickname = IdentityPtr->GetPlayerNickname(UserId);
			UE_LOG(LogTemp, Warning, TEXT("Logged in successfully as: %s"), *(PlayerNickname))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Logging in failed: %s"), *(Error))
		}

		OnLoginCompleted.Broadcast(bWasSuccessful, PlayerNickname, Error);
	}
	else
	{
		OnLoginCompleted.Broadcast(false, "", "Can't find the Identity Pointer");
	}
}

void UGASTPSGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting Create and Join Session: %s"), *(NewSessionName.ToString()))
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchId = FGuid::NewGuid();
	
	FString CoordinatorURL = UGASTPSNetStatics::GetCoordinatorURL();

	FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);
	UE_LOG(LogTemp, Warning, TEXT("Sending Request Session Creation to URL: %s"), *URL)

	Request->SetURL(URL);
	Request->SetVerb("POST");

	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(UGASTPSNetStatics::GetSessionNameKey().ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(UGASTPSNetStatics::GetSessionSearchIdKey().ToString(), SessionSearchId.ToString());

	FString RequestBoby;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBoby);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBoby);
	Request->OnProcessRequestComplete().BindUObject(this, &UGASTPSGameInstance::SessionCreationRequestCompleted, SessionSearchId);
	
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Request Failed Right Away!"))
	}
}

void UGASTPSGameInstance::CancelSessionCreation()
{
	UE_LOG(LogTemp, Warning, TEXT("Canceling Session Creation"))
	StopAllSessionFindings();

	if (IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}

	StartGlobalSessionSearch();
}

void UGASTPSGameInstance::StartGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Global Session Search"))
	GetWorld()->GetTimerManager().SetTimer(GlobalSessionSearchTimerHandle, this, &UGASTPSGameInstance::FindGlobalSessions, GlobalSessionSearchInterval, true, 0.f);
}

bool UGASTPSGameInstance::JoinSessionWithId(const FString& SessionIdStr)
{
	if (SessionSearch.IsValid())
	{
		const FOnlineSessionSearchResult* SessionSearchResult = SessionSearch->SearchResults.FindByPredicate(
			[=](const FOnlineSessionSearchResult& Result)
			{
				return Result.GetSessionIdStr() == SessionIdStr;
			}
		);
		if (SessionSearchResult)
		{
			JoinSessionWithSearchResult(*SessionSearchResult);
			return true;
		}
	}

	return false;
}

void UGASTPSGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully, FGuid SessionSearchId)
{
	if (!bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection responded with connection was not successful!"))
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Connection to Coordinator Successful!"))

	int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Failed, with code: %d"), ResponseCode)
		return;
	}

	FString ResponseStr = Response->GetContentAsString();
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
	int32 Port = 0;

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		Port = JsonObject->GetIntegerField(*(UGASTPSNetStatics::GetPortKey().ToString()));
	}

	UE_LOG(LogTemp, Warning, TEXT("Connected to Coordinator Successfully and the new session created is on port: %d"), Port)
	StartFindingCreatedSession(SessionSearchId);
}

void UGASTPSGameInstance::StartFindingCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Search Id is invalid, can't start finding!"))
		return;
	}

	StopAllSessionFindings();
	UE_LOG(LogTemp, Warning, TEXT("Start Finding Created Sesssion with Id: %s"), *(SessionSearchId.ToString()))

	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimerHandle, 
		FTimerDelegate::CreateUObject(this, &UGASTPSGameInstance::FindCreatedSession, SessionSearchId),
		FindCreatedSessionSearchInterval,
		true, 0.f
		);

	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimeoutTimerHandle, this, &UGASTPSGameInstance::FindCreatedSessionTimeout, FindCreatedSessionTimeoutDuration);
}

void UGASTPSGameInstance::StopAllSessionFindings()
{
	UE_LOG(LogTemp, Warning, TEXT("Stopping All Session Search"))
	StopFindingCreatedSession();
	StopGlobalSessionSearch();
}

void UGASTPSGameInstance::StopFindingCreatedSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Finding Created Session"))
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeoutTimerHandle);

	if (IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

void UGASTPSGameInstance::StopGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Global Session Search"))
	if (GlobalSessionSearchTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GlobalSessionSearchTimerHandle);
	}

	IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this);
	if (SessionPtr)
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UGASTPSGameInstance::FindGlobalSessions()
{
	UE_LOG(LogTemp, Warning, TEXT("Retrying Global Session Search"))

	IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Session Interface, Wait for the next Global Session Search"))
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	
	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UGASTPSGameInstance::GlobalSessionSearchCompleted);
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Global Session Failed Right Away"))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UGASTPSGameInstance::GlobalSessionSearchCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		OnGlobalSessionSearchCompleted.Broadcast(SessionSearch->SearchResults);
		for (const FOnlineSessionSearchResult& OnlineSessionSearchResult : SessionSearch->SearchResults)
		{
			FString SessionName = "Name_None";
			OnlineSessionSearchResult.Session.SessionSettings.Get<FString>(UGASTPSNetStatics::GetSessionNameKey(), SessionName);
			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s after global session search"), *SessionName)
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Global Session Search Completed Unsuccessfully"))
	}

	IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this);
	if (SessionPtr)
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UGASTPSGameInstance::FindCreatedSession(FGuid SessionSearchId)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to find created session"))
	IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find Session Ptr, canceling session search"))
		return;
	}
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	if (!SessionSearch)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to create session search!, canceling session search"))
		return;
	}

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 1;
	SessionSearch->QuerySettings.Set(UGASTPSNetStatics::GetSessionSearchIdKey(), SessionSearchId.ToString(), EOnlineComparisonOp::Equals);

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UGASTPSGameInstance::FindCreateSessionCompleted);
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Session Failed Right Away!..."))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UGASTPSGameInstance::FindCreatedSessionTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("Find Created Session Timeout Reached"))
	StopFindingCreatedSession();
}

void UGASTPSGameInstance::FindCreateSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionSearch->SearchResults.Num() == 0)
	{
		return;
	}

	StopFindingCreatedSession();
	JoinSessionWithSearchResult(SessionSearch->SearchResults[0]);
}

void UGASTPSGameInstance::JoinSessionWithSearchResult(const class FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Joining session with Search Result"))
	IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Session Ptr, Cancel Joining"))
		return;
	}
	
	FString SessionName = "";
	SearchResult.Session.SessionSettings.Get<FString>(UGASTPSNetStatics::GetSessionNameKey(), SessionName);

	const FOnlineSessionSetting* PortSetting = SearchResult.Session.SessionSettings.Settings.Find(UGASTPSNetStatics::GetPortKey());
	int64 Port = 7777;
	PortSetting->Data.GetValue(Port);

	UE_LOG(LogTemp, Warning, TEXT("Trying to join session: %s, at port: %d"), *(SessionName), Port)
	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UGASTPSGameInstance::JoinSessionCompleted, (int)Port);
	if (!SessionPtr->JoinSession(0, FName(SessionName), SearchResult))
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining Session Failed Right Away! ....."))
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		OnJoinSessionFailed.Broadcast();
	}
}

void UGASTPSGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult,
	int Port)
{
	IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this);
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session Completed, but can't find session pointer"))
		OnJoinSessionFailed.Broadcast();
		return;
	}

	if (JoinResult == EOnJoinSessionCompleteResult::Success)
	{
		StopAllSessionFindings();
		UE_LOG(LogTemp, Warning, TEXT("Joining Session: %s successful, the port is: %d"), *(SessionName.ToString()), Port)

		FString TravelURL = "";
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);
		
		FString TestingURL = UGASTPSNetStatics::GetTestingURL();
		if (!TestingURL.IsEmpty())
		{
			TravelURL = TestingURL;
		}

		UGASTPSNetStatics::ReplacePort(TravelURL, Port);

		UE_LOG(LogTemp, Warning, TEXT("Traveling to Session at: %s"), *TravelURL)

		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		OnJoinSessionFailed.Broadcast();
	}

	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
}

void UGASTPSGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	}

	PlayerRecord.Add(UniqueId);
}

void UGASTPSGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All player left the session, terminating"))
		TerminateSessionServer();
	}
}

void UGASTPSGameInstance::CreateSession()
{
	IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this);
	if (SessionPtr)
	{
		ServerSessionName = UGASTPSNetStatics::GetSessionNameStr();
		FString SessionSearchId = UGASTPSNetStatics::GetSessionSearchIdStr();
		SessionServerPort = UGASTPSNetStatics::GetSessionPort();
		UE_LOG(LogTemp, Warning, TEXT("Create Session With Name: %s, ID: %s, Port: %d"), *(ServerSessionName), *(SessionSearchId), SessionServerPort)

		FOnlineSessionSettings OnlineSessionSetting = UGASTPSNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, SessionServerPort);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UGASTPSGameInstance::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSetting))
		{
			UE_LOG(LogTemp, Warning, TEXT("!!!!Session Creation Failed Instantly!!!!"))
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find SessionPtr, terminating"))
		TerminateSessionServer();
	}
}

void UGASTPSGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("!Session Created! Now starting session..."))

		// Start the session to register it with EOS backend so it becomes visible to other players
		if (IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this))
		{
			SessionPtr->OnStartSessionCompleteDelegates.AddUObject(this, &UGASTPSGameInstance::OnSessionStarted);
			SessionPtr->StartSession(SessionName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("!Session Creation Failed!"))
		TerminateSessionServer();
	}

	if (IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UGASTPSGameInstance::OnSessionStarted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("!Session Started Successfully! Session is now visible to other players"))
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimeoutHandle, this, &UGASTPSGameInstance::WaitPlayerJoinTimeoutReached, WaitPlayerJoinTimeOutDuration);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("!Session Start Failed!"))
		TerminateSessionServer();
	}

	if (IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnStartSessionCompleteDelegates.RemoveAll(this);
	}
}

void UGASTPSGameInstance::EndSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UGASTPSGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UGASTPSNetStatics::GetSessionPtr(this))
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UGASTPSGameInstance::EndSessionCompleted);
		if (!SessionPtr->EndSession(FName{ ServerSessionName }))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UGASTPSGameInstance::WaitPlayerJoinTimeoutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("Session Server shut down after %f seconds without player joining"), WaitPlayerJoinTimeOutDuration)
	TerminateSessionServer();
}

void UGASTPSGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));

	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), SessionServerPort);
		UE_LOG(LogTemp, Warning, TEXT("Server Traveling to: %s"), *(TravelStr))
		GetWorld()->ServerTravel(TravelStr);
	}
}
