// Fill out your copyright notice in the Description page of Project Settings.

#include "HotStuffGameState.h"
#include "HotStuffPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "HotStuffGameMode.h"
#include "UserInterface/InGameMatchInfo.h"

const static int FIRST_TEAM_NUM = 0;
const static int SECOND_TEAM_NUM = 1;

const static int NUM_TEAMS = 2;

void AHotStuffGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TeamScores.SetNum(NUM_TEAMS);
}

void AHotStuffGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHotStuffGameState, TeamScores)
	DOREPLIFETIME(AHotStuffGameState, MatchState)
	DOREPLIFETIME(AHotStuffGameState, RemainingTime)
}

void AHotStuffGameState::UpdateScore(int32 Points, int32 TeamNum)
{
	if (TeamNum >= NUM_TEAMS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Received invalid team number in UpdateScore"))
		return;
	}

	TeamScores[TeamNum] += Points;
	
	if (HasAuthority())
	{
		// does not trigger automatically on the server
		OnRep_Score();

		if (MatchState == "Overtime")
		{
			auto GameMode = Cast<AHotStuffGameMode>(AuthorityGameMode);
			if (!ensure(GameMode)) return;

			GameMode->GameOver(TeamNum);
		}
	}
}

TArray<int32> AHotStuffGameState::GetTeamScores()
{
	return TeamScores;
}

void AHotStuffGameState::MulticastGoalNotification_Implementation(int32 ScoringTeam)
{
	if (!ensure(InGameMatchInfoWidget)) return;

	auto MyPlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	auto MyPlayerState = Cast<AHotStuffPlayerState>(MyPlayerController->PlayerState);
	if (!ensure(MyPlayerState)) return;

	InGameMatchInfoWidget->ShowGoalMessage(MyPlayerState->GetTeamNumber() != ScoringTeam);
}

void AHotStuffGameState::OnRep_Score()
{
	if (!ensure(InGameMatchInfoWidget)) return;

	auto MyPlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	auto MyPlayerState = Cast<AHotStuffPlayerState>(MyPlayerController->PlayerState);
	if (!ensure(MyPlayerState)) return;

	switch (MyPlayerState->GetTeamNumber())
	{
	case FIRST_TEAM_NUM:
		InGameMatchInfoWidget->UpdateScores(TeamScores[FIRST_TEAM_NUM], TeamScores[SECOND_TEAM_NUM]);
		break;
	case SECOND_TEAM_NUM:
		InGameMatchInfoWidget->UpdateScores(TeamScores[SECOND_TEAM_NUM], TeamScores[FIRST_TEAM_NUM]);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("%s has no valid team assigned"), *MyPlayerState->GetName())
		break;
	}
}

void AHotStuffGameState::OnRep_TimeCounter()
{
	if (InGameMatchInfoWidget)
	{
		InGameMatchInfoWidget->UpdateTimerDisplay(RemainingTime);
	}
}

void AHotStuffGameState::GameOver_Implementation(int32 WinnerTeam)
{
	auto PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (!ensure(PlayerController)) return;

	auto HSPlayerState = Cast<AHotStuffPlayerState>(PlayerController->PlayerState);
	if (!ensure(HSPlayerState)) return;

	if (!ensure(InGameMatchInfoWidget)) return;

	FString NoobsMessage;
	for (auto Player : GetNoobsPlayerState(WinnerTeam))
	{
		NoobsMessage.Append(Player->GetPlayerName() + " ");
	}

	NoobsMessage.Append("needs to Git Gud!");

	InGameMatchInfoWidget->ShowMatchOutcome(HSPlayerState->GetTeamNumber() == WinnerTeam);
	InGameMatchInfoWidget->ShowMessage(NoobsMessage);
}

int32 AHotStuffGameState::GetWinningTeam()
{
	// first team winning
	if (TeamScores[0] > TeamScores[1])
	{
		return 0;
	}
	// second team winning
	if (TeamScores[0] < TeamScores[1])
	{
		return 1;
	}
	// tie
	return -1;
}

void AHotStuffGameState::StartCountDown(int32 Seconds)
{
	RemainingTime = Seconds;
	GetWorldTimerManager().SetTimer(CountDownTimer, this, &AHotStuffGameState::CountDown, 1.0f, true);
}

void AHotStuffGameState::CountDown()
{
	RemainingTime--;
	// Otherwise server does not executes
	OnRep_TimeCounter();

	if (RemainingTime == 0)
	{
		GetWorldTimerManager().ClearTimer(CountDownTimer);
	}
}

TArray<APlayerState*> AHotStuffGameState::GetNoobsPlayerState(int32 WinningTeam) const
{
	int32 MaxDeaths = 0;

	for (auto PlayerState : PlayerArray)
	{
		auto HSPlayerState = Cast<AHotStuffPlayerState>(PlayerState);
		if (!ensure(HSPlayerState)) return PlayerArray;

		MaxDeaths = FMath::Max(MaxDeaths, HSPlayerState->GetNumDeaths());
	}

	return PlayerArray.FilterByPredicate([&](APlayerState* PlayerState) 
	{
		auto HSPlayerState = Cast<AHotStuffPlayerState>(PlayerState);
		return HSPlayerState->GetNumDeaths() == MaxDeaths && HSPlayerState->GetTeamNumber() != WinningTeam;
	});
}