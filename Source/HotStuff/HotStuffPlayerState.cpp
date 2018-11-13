// Fill out your copyright notice in the Description page of Project Settings.

#include "HotStuffPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

#include "LobbyPlayerController.h"
#include "UserInterface/LobbyMenu.h"
#include "Drone.h"

void AHotStuffPlayerState::ClientInitialize(AController * C)
{
	Super::ClientInitialize(C);

	auto LobbyController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	
	if (LobbyController && LobbyController->LobbyMenuWidget)
	{
		LobbyController->LobbyMenuWidget->RefreshPlayers();
	}

	auto OwnerController = Cast<AController>(GetOwner());
	if (!OwnerController) return;

	auto Pawn = Cast<ADrone>(OwnerController->GetPawn());
	if (!Pawn) return;

	Pawn->UpdateName();
	Pawn->UpdateMeshColor();
}

void AHotStuffPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	auto LobbyController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());

	if (LobbyController && LobbyController->LobbyMenuWidget)
	{
		LobbyController->LobbyMenuWidget->AddPlayerProfile(GetPlayerName(), TeamNumber);
	}

	auto OwnerController = Cast<AController>(GetOwner());
	if (!OwnerController) return;

	auto Pawn = Cast<ADrone>(OwnerController->GetPawn());
	if (!Pawn) return;

	Pawn->UpdateName();
}

void AHotStuffPlayerState::OnRep_TeamNumber()
{
	auto OwnerController = Cast<AController>(GetOwner());
	if (!OwnerController) return;

	auto Pawn = Cast<ADrone>(OwnerController->GetPawn());
	if (!Pawn) return;

	Pawn->UpdateMeshColor();
	Pawn->UpdateCollisionChannel();
}


void AHotStuffPlayerState::CopyProperties(APlayerState * PlayerState)
{
	Super::CopyProperties(PlayerState);

	AHotStuffPlayerState* HotStuffPlayer = Cast<AHotStuffPlayerState>(PlayerState);
	if (!ensure(HotStuffPlayer)) return;

	HotStuffPlayer->TeamNumber = TeamNumber;
}

int32 AHotStuffPlayerState::GetTeamNumber()
{
	return TeamNumber;
}

int32 AHotStuffPlayerState::GetNumDeaths()
{
	return NumDeaths;
}

void AHotStuffPlayerState::SetTeamNumber(int32 TeamNumber)
{
	this->TeamNumber = TeamNumber;
}

void AHotStuffPlayerState::ScoreDeath()
{
	NumDeaths++;
}

void AHotStuffPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHotStuffPlayerState, TeamNumber)
	DOREPLIFETIME(AHotStuffPlayerState, NumDeaths)
}
