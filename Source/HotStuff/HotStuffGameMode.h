#pragma once
#include "GameFramework/GameModeBase.h"
#include "HotStuffGameMode.generated.h"

class APlayerStart;
class ADroneController;
class AStaticMeshActor;
class ATeamBase;

UCLASS(minimalapi)
class AHotStuffGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHotStuffGameMode();

	void InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage) override;

	void PostSeamlessTravel() override;

	AActor* ChoosePlayerStart_Implementation(AController * Player) override;

	void BeginPlay() override;

	void StartGame();

	void Overtime();

	void GameOver(int32 WinnerTeam);

	void CloseMap();

	UFUNCTION(BlueprintCallable)
		void HandleDroneKilled(AController* DeadPlayer, AController* KillerPlayer = nullptr);

	UFUNCTION(BlueprintCallable)
		void HandleDestroyBase(ATeamBase* BaseToDestroy);

	UFUNCTION()
		void HandleRespawnBase(FVector BaseLocation, FRotator BaseRotation, int32 TeamNumber);

	UFUNCTION(Exec)
		void RestartGameExec();

	UFUNCTION(Exec)
		void RespawnBomb();

private:

	void OpenGates();

	TArray<APlayerStart*> FirstTeamSpawnPoints;
	TArray<APlayerStart*> SecondTeamSpawnPoints;

	int FirstSpawnPointIndex = 0;
	int SecondSpawnPointIndex = 0;

	FTimerHandle PreGameTimer;
	FTimerHandle GameTimer;
	FTimerHandle EndGameTimer;

	UPROPERTY(EditAnywhere, Category = "Match Schedule")
		float PreGameDuration = 10;
	UPROPERTY(EditAnywhere, Category = "Match Schedule")
		float GameDuration = 600;
	UPROPERTY(EditAnywhere, Category = "Match Schedule")
		float EndGameDuration = 10;	
	UPROPERTY(EditAnywhere, Category = "Match Schedule")
		float DroneRespawnDuration = 5;
	UPROPERTY(EditAnywhere, Category = "Match Schedule")
		float BaseRespawnDuration = 5;

	UPROPERTY(EditAnywhere, Category = "Scores")
		int32 PointsPerDestroyedDrone = 10;
	UPROPERTY(EditAnywhere, Category = "Scores")
		int32 PointsPerDestroyedBase = 100;

	UPROPERTY(EditAnywhere, Category = "Game Classes")
		TSubclassOf<AStaticMeshActor> TeamBaseClass;
	UPROPERTY(EditAnywhere, Category = "Game Classes")
		TSubclassOf<AStaticMeshActor> BombClass;

	// Needed when travel back to lobby
	int32 NumPlayers;
};
