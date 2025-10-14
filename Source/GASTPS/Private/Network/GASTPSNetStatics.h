// VyniosNikolaos, Thesis, University of Patras, 2025

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "GASTPSNetStatics.generated.h"

/**
 * 
 */
UCLASS()
class UGASTPSNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:	
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int Port);

	static IOnlineSessionPtr GetSessionPtr(const UObject* WorldContextObject, FName SubsystemName = NAME_None);
	static IOnlineIdentityPtr GetIdentityPtr(const UObject* WorldContextObject, FName SubsystemName = NAME_None);

	static uint8 GetPlayerCountPerTeam();

	static bool IsSessionServer(const UObject* WorldContextObject);

	static FString GetSessionNameStr();
	static FName GetSessionNameKey();

	static FString GetSessionSearchIdStr();
	static FName GetSessionSearchIdKey();

	static int GetSessionPort();
	static FName GetPortKey();

	static FName GetCoordinatorURLKey();
	static FString GetCoordinatorURL();
	static FString GetDefaultCoordinatorURL();

	static FString GetCommandlineArgAsString(const FName& ParamName);
	static int GetCommandlineArgAsInt(const FName& ParamName);

	static FString GetTestingURL();
	static FName GetTestingURLKey();

	static void ReplacePort(FString& OutURLStr, int NewPort);
};
