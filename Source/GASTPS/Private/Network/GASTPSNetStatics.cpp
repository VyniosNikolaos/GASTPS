// VyniosNikolaos, Thesis, University of Patras, 2025


#include "GASTPSNetStatics.h"
#include "OnlineSubsystemUtils.h"


FOnlineSessionSettings UGASTPSNetStatics::GenerateOnlineSessionSettings(const FName& SessionName,
	const FString& SessionSearchId, int Port)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = false;
	OnlineSessionSettings.NumPublicConnections = GetPlayerCountPerTeam() * 2;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.bUsesPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinInProgress = true;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;
	
	OnlineSessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetSessionSearchIdKey(), SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return OnlineSessionSettings;
}

IOnlineSessionPtr UGASTPSNetStatics::GetSessionPtr(const UObject* WorldContextObject, FName SubsystemName /*= NAME_None*/)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	IOnlineSubsystem* OSS = Online::GetSubsystem(World, SubsystemName);
	return OSS ? OSS->GetSessionInterface() : nullptr;
}

IOnlineIdentityPtr UGASTPSNetStatics::GetIdentityPtr(const UObject* WorldContextObject, FName SubsystemName /*= NAME_None*/)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	IOnlineSubsystem* OSS = Online::GetSubsystem(World, SubsystemName);
	return OSS ? OSS->GetIdentityInterface() : nullptr;
}

uint8 UGASTPSNetStatics::GetPlayerCountPerTeam()
{
	return 5;
}

bool UGASTPSNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer;
}

FString UGASTPSNetStatics::GetSessionNameStr()
{
	return GetCommandlineArgAsString(GetSessionNameKey());
}

FName UGASTPSNetStatics::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UGASTPSNetStatics::GetSessionSearchIdStr()
{
	return GetCommandlineArgAsString(GetSessionSearchIdKey());
}

FName UGASTPSNetStatics::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UGASTPSNetStatics::GetSessionPort()
{
	return GetCommandlineArgAsInt(GetPortKey());
}

FName UGASTPSNetStatics::GetPortKey()
{
	return FName("PORT");
}

FName UGASTPSNetStatics::GetCoordinatorURLKey()
{
	return FName("COORDINATOR_URL");
}

FString UGASTPSNetStatics::GetCoordinatorURL()
{
	FString CoordinatorURL = GetCommandlineArgAsString(GetCoordinatorURLKey());
	if (CoordinatorURL != "")
	{
		return CoordinatorURL;
	}

	return GetDefaultCoordinatorURL();
}

FString UGASTPSNetStatics::GetDefaultCoordinatorURL()
{
	FString CoordinatorURL = "";
	GConfig->GetString(TEXT("GASTPS.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);
	UE_LOG(LogTemp, Warning, TEXT("Getting Default Coordinator URL as: %s"), *CoordinatorURL)
	return CoordinatorURL;
}

FString UGASTPSNetStatics::GetCommandlineArgAsString(const FName& ParamName)
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UGASTPSNetStatics::GetCommandlineArgAsInt(const FName& ParamName)
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

FString UGASTPSNetStatics::GetTestingURL()
{
	FString TestURL = GetCommandlineArgAsString(GetTestingURLKey());
	UE_LOG(LogTemp, Warning, TEXT("Get Testing URL: %s"), *TestURL)
	return TestURL;
}

FName UGASTPSNetStatics::GetTestingURLKey()
{
	return FName("TESTING_URL");
}

void UGASTPSNetStatics::ReplacePort(FString& OutURLStr, int NewPort)
{
	FURL URL(nullptr, *OutURLStr, ETravelType::TRAVEL_Absolute);
	URL.Port = NewPort;
	OutURLStr = URL.ToString();
}