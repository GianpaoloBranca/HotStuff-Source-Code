// Fill out your copyright notice in the Description page of Project Settings.

#include "HotStuffGameInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

#include "OnlineSessionSettings.h"

#include "UserInterface/MainMenu.h"
#include "UserInterface/LobbyMenu.h"
#include "LobbyGameMode.h"

const static FName SESSION_NAME = TEXT("Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

void UHotStuffGameInstance::Init()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!ensure(OnlineSubsystem)) return;

	UE_LOG(LogTemp, Warning, TEXT("Found Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString())

	SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UHotStuffGameInstance::OnCreateSessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UHotStuffGameInstance::OnFindSessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UHotStuffGameInstance::OnJoinSessionComplete);

}

void UHotStuffGameInstance::Host(FString ServerName, int32 NumPlayers)
{
	DesiredServerName = ServerName;

	// idiot proof check, only 2 to 6 players allowed
	if (NumPlayers <= 1 || NumPlayers  >= 7)
	{
		DesiredNumPlayers = DefaultNumPlayers;
	}
	else
	{
		DesiredNumPlayers = NumPlayers;
	}

	if (!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);

	if (!ExistingSession)
	{
		FOnlineSessionSettings SessionSettings;
		// If the subsystem is the NULL_SUBSYSTEM, Lan match is set to true
		SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == NULL_SUBSYSTEM;
		SessionSettings.NumPublicConnections = DesiredNumPlayers;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;

		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
	else
	{
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UHotStuffGameInstance::OnDestroySessionComplete);
		SessionInterface->DestroySession(SESSION_NAME);
	}
}

void UHotStuffGameInstance::Join(FString UserName, uint32 Index)
{
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid()) return;

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UHotStuffGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}

void UHotStuffGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->MaxSearchResults = 100;
		//SessionSearch->bIsLanQuery = true;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		UE_LOG(LogTemp, Warning, TEXT("Searching for Sessions"))
	}
}

APlayerController * UHotStuffGameInstance::GetMyPlayerController()
{
	return GetFirstLocalPlayerController();
}

void UHotStuffGameInstance::QuitToMainMenu()
{
	auto PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController)) return;

	PlayerController->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UHotStuffGameInstance::QuitGame()
{
	auto PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController)) return;

	PlayerController->ConsoleCommand("quit");
}

void UHotStuffGameInstance::SetMainMenuWidget(UMainMenu* Menu)
{
	if (!ensure(Menu)) return;

	MainMenuWidget = Menu;
	MainMenuWidget->SetMenuInterface(this);
}

void UHotStuffGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create Session"))
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine)) return;

	Engine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World)) return;

	FString AddressWithArgument = FString::Printf(TEXT("/Game/Maps/Lobby?Listen?NumPlayers=%d"), DesiredNumPlayers);
	World->ServerTravel(AddressWithArgument);
}

void UHotStuffGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		Host(DesiredServerName, DesiredNumPlayers);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not Delete Session"))
	}

	SessionInterface->OnDestroySessionCompleteDelegates.Clear();
}

void UHotStuffGameInstance::OnFindSessionComplete(bool Success)
{
	if (Success && SessionSearch.IsValid()) {

		TArray<FServerData> ServerDatas;

		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FServerData Data;

			Data.HostUserName = SearchResult.Session.OwningUserName;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;

			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No data found in settings"))
				Data.Name = SearchResult.GetSessionIdStr();
			}
			ServerDatas.Add(Data);
		}

		UE_LOG(LogTemp, Warning, TEXT("Search completed"))

		if (!ensure(MainMenuWidget)) return;
		MainMenuWidget->SetServerList(ServerDatas);
	}

}

void UHotStuffGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Address;
		if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not get the connect string"))
				return;
		}
		UEngine * Engine = GetEngine();
		if (!ensure(Engine)) return;

		Engine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Joining: " + Address));

		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (!ensure(PlayerController)) return;

		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	} 
}

void UHotStuffGameInstance::JoinExec(FString Address)
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}
