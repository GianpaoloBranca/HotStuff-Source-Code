#include "HotStuffGameMode.h"
#include "DroneController.h"
#include "HotStuffPlayerState.h"
#include "HotStuffGameState.h"
#include "TeamBase.h"
#include "SpawnZoneDelimiter.h"

#include "Engine/World.h"
#include "Engine/TargetPoint.h"
#include "TimerManager.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

#include "Blueprint/WidgetLayoutLibrary.h"

const static FName FIRST_TEAM_NAME = TEXT("Team_0");
const static FName SECOND_TEAM_NAME = TEXT("Team_1");

const static int FIRST_TEAM_NUM = 0;
const static int SECOND_TEAM_NUM = 1;

AHotStuffGameMode::AHotStuffGameMode()
{
	GameStateClass = AHotStuffGameState::StaticClass();
	PlayerStateClass = AHotStuffPlayerState::StaticClass();
}

void AHotStuffGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString NumPlayersOpt = UGameplayStatics::ParseOption(Options, "NumPlayers");
	FString PlayTimeOpt = UGameplayStatics::ParseOption(Options, "PlayTime");

	if (!PlayTimeOpt.IsEmpty() && PlayTimeOpt.IsNumeric())
	{
		GameDuration = FCString::Atoi(*PlayTimeOpt);
	}

	if (!NumPlayersOpt.IsEmpty() && NumPlayersOpt.IsNumeric())
	{
		NumPlayers = FCString::Atoi(*NumPlayersOpt);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Received num players: %d, playtime: %d"), NumPlayers, GameDuration);
}

void AHotStuffGameMode::PostSeamlessTravel()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	UE_LOG(LogTemp, Log, TEXT("%d Spawn points found"), Actors.Num())

	for (auto Actor : Actors)
	{
		auto PlayerStart = Cast<APlayerStart>(Actor);

		if (PlayerStart->PlayerStartTag == FIRST_TEAM_NAME)
		{
			FirstTeamSpawnPoints.Add(PlayerStart);
		}
		else if (PlayerStart->PlayerStartTag == SECOND_TEAM_NAME)
		{
			SecondTeamSpawnPoints.Add(PlayerStart);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Found PlayerStart not properly tagged"))
		}
	}

	Super::PostSeamlessTravel();

	RespawnBomb();
}

AActor* AHotStuffGameMode::ChoosePlayerStart_Implementation(AController * Player)
{ 
	if (FirstTeamSpawnPoints.Num() == 0 || SecondTeamSpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing a spawn point for one of the team. Using the standard ChoosePlayerStart function"))
		UE_LOG(LogTemp, Warning, TEXT("Ignore the previous warning if testing without connection"))
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	auto PlayerState = Cast<AHotStuffPlayerState>(Player->PlayerState);
	if (!ensure(PlayerState)) return nullptr;

	switch (PlayerState->GetTeamNumber())
	{
	case FIRST_TEAM_NUM:
		FirstSpawnPointIndex = (FirstSpawnPointIndex + 1) % FirstTeamSpawnPoints.Num();
		return FirstTeamSpawnPoints[FirstSpawnPointIndex];
	case SECOND_TEAM_NUM:
		SecondSpawnPointIndex = (SecondSpawnPointIndex + 1) % SecondTeamSpawnPoints.Num();
		return SecondTeamSpawnPoints[SecondSpawnPointIndex];
	default:
		UE_LOG(LogTemp, Warning, TEXT("%s has no valid team assigned"), *PlayerState->GetName())
		return nullptr;
	}	
}

void AHotStuffGameMode::BeginPlay()
{
	auto HSGameState = Cast<AHotStuffGameState>(GameState);
	if (!ensure(HSGameState)) return;

	HSGameState->MatchState = "Startup";
	HSGameState->StartCountDown(PreGameDuration);

	GetWorldTimerManager().SetTimer(PreGameTimer, this, &AHotStuffGameMode::StartGame, PreGameDuration);
}

void AHotStuffGameMode::StartGame()
{
	auto HSGameState = Cast<AHotStuffGameState>(GameState);
	if (!ensure(HSGameState)) return;

	OpenGates();

	HSGameState->MatchState = "Fighting";
	HSGameState->StartCountDown(GameDuration);

	GetWorldTimerManager().SetTimer(GameTimer, this, &AHotStuffGameMode::Overtime, GameDuration);
}

void AHotStuffGameMode::Overtime()
{
	auto HSGameState = Cast<AHotStuffGameState>(GameState);
	if (!ensure(HSGameState)) return;

	int32 WinnerTeam = HSGameState->GetWinningTeam();
	// Draw
	if (WinnerTeam < 0)
	{
		// Stay here, GameOver will be triggered by the first team that make some points
		HSGameState->MatchState = "Overtime";
	}
	else
	{
		GameOver(WinnerTeam);
	}
}

void AHotStuffGameMode::GameOver(int32 WinnerTeam)
{
	if (WinnerTeam < 0 || WinnerTeam >= 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameOver: Invalid Winner Team"))
	}

	auto HSGameState = Cast<AHotStuffGameState>(GameState);
	if (!ensure(HSGameState)) return;

	HSGameState->MatchState = "Gameover";
	HSGameState->GameOver(WinnerTeam);
	HSGameState->StartCountDown(EndGameDuration);

	GetWorldTimerManager().SetTimer(EndGameTimer, this, &AHotStuffGameMode::CloseMap, EndGameDuration);
}

void AHotStuffGameMode::CloseMap()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	bUseSeamlessTravel = true;

	FString AddressWithArgument = FString::Printf(TEXT("/Game/Maps/Lobby?Listen?NumPlayers=%d"), NumPlayers);
	GetWorld()->ServerTravel(AddressWithArgument);
}

void AHotStuffGameMode::HandleDroneKilled(AController * DeadPlayer, AController* KillerPlayer)
{
	FTimerHandle RespawnTimer;
	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUFunction(this, FName("RestartPlayer"), DeadPlayer);

	GetWorldTimerManager().SetTimer(RespawnTimer, RespawnDelegate, DroneRespawnDuration, false);

	auto DeadPlayerState = Cast<AHotStuffPlayerState>(DeadPlayer->PlayerState);
	if (!ensure(DeadPlayerState)) return;

	DeadPlayerState->ScoreDeath();

	if (KillerPlayer)
	{
		auto KillerDroneController = Cast<ADroneController>(KillerPlayer);
		if (!ensure(KillerDroneController)) return;

		auto KillerPlayerState = Cast<AHotStuffPlayerState>(KillerDroneController->PlayerState);
		if (!ensure(KillerPlayerState)) return;	
		// Normal kill
		if (KillerPlayerState->GetTeamNumber() != DeadPlayerState->GetTeamNumber())
		{
			auto HSGameState = Cast<AHotStuffGameState>(GameState);
			if (!ensure(HSGameState)) return;

			KillerDroneController->ClientMarkKill(DeadPlayerState->GetPlayerName(), false);

			HSGameState->UpdateScore(PointsPerDestroyedDrone, KillerPlayerState->GetTeamNumber());
		}
		// Friendly kill
		else
		{
			KillerDroneController->ClientMarkKill(DeadPlayerState->GetPlayerName(), true);
		}	
	}	
}

void AHotStuffGameMode::HandleDestroyBase(ATeamBase * BaseToDestroy)
{
	FVector BaseLocation = BaseToDestroy->GetActorLocation();
	FRotator BaseRotation = BaseToDestroy->GetActorRotation();
	int32 BaseTeamNum = BaseToDestroy->TeamNumber;

	BaseToDestroy->Destroy(true);

	auto HSGameState = Cast<AHotStuffGameState>(GameState);
	if (!ensure(HSGameState)) return;

	switch (BaseTeamNum)
	{
	case FIRST_TEAM_NUM:
		HSGameState->UpdateScore(PointsPerDestroyedBase, SECOND_TEAM_NUM);
		HSGameState->MulticastGoalNotification(SECOND_TEAM_NUM);
		break;
	case SECOND_TEAM_NUM:
		HSGameState->UpdateScore(PointsPerDestroyedBase, FIRST_TEAM_NUM);
		HSGameState->MulticastGoalNotification(FIRST_TEAM_NUM);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("The base destroyed had no proper team number assigned"))
		break;
	}

	FTimerHandle RespawnTimer;
	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUFunction(this, FName("HandleRespawnBase"), BaseLocation, BaseRotation, BaseTeamNum);

	GetWorldTimerManager().SetTimer(RespawnTimer, RespawnDelegate, BaseRespawnDuration, false);
}

void AHotStuffGameMode::HandleRespawnBase(FVector BaseLocation, FRotator BaseRotation, int32 TeamNumber)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto Base = GetWorld()->SpawnActor<ATeamBase>(TeamBaseClass, BaseLocation, BaseRotation, SpawnInfo);

	Base->TeamNumber = TeamNumber;
	Base->UpdateMeshColor();
}

void AHotStuffGameMode::RestartGameExec()
{
	CloseMap();
}

void AHotStuffGameMode::RespawnBomb()
{
	TArray<AActor*> Bombs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BombClass, Bombs);

	for (auto Bomb : Bombs)
	{
		Bomb->Destroy(true, true);
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	TArray<AActor*> TargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), TargetPoints);
	
	if (TargetPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No target points in the level: Cannot spawn bomb"))
		return;
	}

	int32 index = FMath::RandRange(0, TargetPoints.Num() - 1);
	GetWorld()->SpawnActor<AStaticMeshActor>(BombClass, TargetPoints[index]->GetTransform(), SpawnInfo);
}

void AHotStuffGameMode::OpenGates()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnZoneDelimiter::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		Cast<ASpawnZoneDelimiter>(Actor)->MulticastOpenGate();
	}
}
