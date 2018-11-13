// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

#include "HotStuffGameState.h"
#include "HotStuffPlayerState.h"

#include "HotStuffGameInstance.h"
#include "UObject/ConstructorHelpers.h"

#include "Blueprint/UserWidget.h"
#include "UserInterface/LobbyMenu.h"
#include "LobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"

const static FString PARAM_NUM_PLAYERS = "NumPlayers";

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass = AHotStuffGameState::StaticClass();
}

void ALobbyGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString NumPlayers = UGameplayStatics::ParseOption(Options, PARAM_NUM_PLAYERS);
	
	if (NumPlayers.IsNumeric())
	{
		DesiredPlayersToStart = FCString::Atoi(*NumPlayers);
	}

	UE_LOG(LogTemp, Warning, TEXT("Required players to start: %d"), DesiredPlayersToStart);
}

void ALobbyGameMode::GenericPlayerInitialization(AController * NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	auto NewPlayerController = Cast<ALobbyPlayerController>(NewPlayer);
	if (!ensure(NewPlayerController)) return;

	ConnectedPlayersControllers.AddUnique(NewPlayerController);

	auto NewPlayerState = CastChecked<AHotStuffPlayerState>(NewPlayer->PlayerState);
	const uint16 TeamNumber = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNumber(TeamNumber);
}

void ALobbyGameMode::Logout(AController * Exiting)
{
	Super::Logout(Exiting);

	GameState->PlayerArray.Remove(Exiting->PlayerState);

	auto ExitingPlayer = Cast<ALobbyPlayerController>(Exiting);
	if (ExitingPlayer)
	{
		UE_LOG(LogTemp, Log, TEXT("Logging out %s"), *ExitingPlayer->PlayerState->GetPlayerName())

		ConnectedPlayersControllers.Remove(ExitingPlayer);
		
		if (ConnectedPlayersControllers.Num() < DesiredPlayersToStart)
		{
			for (auto Player : ConnectedPlayersControllers)
			{
				Player->ClientCanBeReady(false);
			}
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Logging out something else than a playercontroller"))
	}

}

void ALobbyGameMode::SetPlayerReady(APlayerController * ReadyPlayer, bool Value)
{
	FString PlayerName = ReadyPlayer->PlayerState->GetPlayerName();
	int32 NumReadyPlayers = 0;

	for (auto PlayerController : ConnectedPlayersControllers)
	{
		PlayerController->ClientSetPlayerReady(PlayerName, Value);

		NumReadyPlayers += PlayerController->bReady;
	}

	if (NumReadyPlayers == DesiredPlayersToStart)
	{
		for (auto PlayerController : ConnectedPlayersControllers)
		{
			PlayerController->ClientStartingMatch(CountdownToStart);
		}

		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, CountdownToStart);
	}
}

void ALobbyGameMode::PostClientReadyNotification(ALobbyPlayerController* Notifier)
{
	for (auto Player : ConnectedPlayersControllers)
	{
		Player->ClientRefreshPlayersList();
	}

	if (ConnectedPlayersControllers.Num() == DesiredPlayersToStart)
	{
		for (auto Player : ConnectedPlayersControllers)
		{
			Player->ClientCanBeReady(true);
		}
	}
}

void ALobbyGameMode::SetPlayTimeExec(int32 PlayTime)
{
	bOverrideDefaultPlayTime = true;
	DesiredPlayTime = PlayTime;
}

void ALobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UHotStuffGameInstance>(GetGameInstance());
	if (!ensure(GameInstance)) return;

	GameInstance->StartSession();

	UE_LOG(LogTemp, Log, TEXT("Starting Session"))
	bUseSeamlessTravel = true;

	FString AddressWithArgument = "/Game/VehicleCPP/Maps/TestMap?Listen";

	AddressWithArgument.Append(FString::Printf(TEXT("?NumPlayers=%d"), DesiredPlayersToStart));

	if (bOverrideDefaultPlayTime)
	{
		AddressWithArgument.Append(FString::Printf(TEXT("?PlayTime=%d"), DesiredPlayTime));
	}

	GetWorld()->ServerTravel(AddressWithArgument);
}


// credits for this function: 
// https://github.com/Noesis/UE4-ShooterGame/blob/master/Source/ShooterGame/Private/Online/ShooterGame_TeamDeathMatch.cpp
int32 ALobbyGameMode::ChooseTeam(AHotStuffPlayerState* ForPlayerState) const
{
	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(NumTeams);

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		auto TestPlayerState = Cast<AHotStuffPlayerState>(GameState->PlayerArray[i]);

		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamNumber()))
		{
			TeamBalance[TestPlayerState->GetTeamNumber()]++;
		}
	}

	// find least populated one
	int32 BestTeamScore = TeamBalance[0];
	for (int32 i = 1; i < TeamBalance.Num(); i++)
	{
		if (BestTeamScore > TeamBalance[i])
		{
			BestTeamScore = TeamBalance[i];
		}
	}

	// there could be more than one...
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamBalance.Num(); i++)
	{
		if (TeamBalance[i] == BestTeamScore)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return RandomBestTeam;
}
