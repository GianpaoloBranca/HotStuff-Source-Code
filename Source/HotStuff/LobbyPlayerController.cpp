// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPlayerController.h"
#include "LobbyGameMode.h"
#include "UserInterface/LobbyMenu.h"

bool ALobbyPlayerController::ServerReady_Validate(bool Value)
{
	return true;
}

void ALobbyPlayerController::ServerReady_Implementation(bool Value)
{
	ALobbyGameMode* GameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (!ensure(GameMode)) return;

	// set myself to ready (or not)
	bReady = Value;

	GameMode->SetPlayerReady(this, Value);
}

bool ALobbyPlayerController::ServerNotifyMenuInitialized_Validate()
{
	return true;
}

void ALobbyPlayerController::ServerNotifyMenuInitialized_Implementation()
{
	auto GameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (!ensure(GameMode)) return;

	GameMode->PostClientReadyNotification(this);
}

void ALobbyPlayerController::ClientSetPlayerReady_Implementation(const FString& PlayerName, bool Value)
{
	if (!LobbyMenuWidget) return;
	LobbyMenuWidget->SetPlayerReady(PlayerName, Value);
}

void ALobbyPlayerController::ClientCanBeReady_Implementation(bool Value)
{
	if (!LobbyMenuWidget) return;
	LobbyMenuWidget->CanBeReady(Value);
}

void ALobbyPlayerController::ClientStartingMatch_Implementation(float RemainingSeconds)
{
	if (!LobbyMenuWidget) return;
	LobbyMenuWidget->OnStartingMatch(RemainingSeconds);
}

void ALobbyPlayerController::ClientRefreshPlayersList_Implementation()
{
	if (!LobbyMenuWidget) return;
	LobbyMenuWidget->RefreshPlayers();
}
