// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	void InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage) override;

	void GenericPlayerInitialization(AController* NewPlayer) override;

	void Logout(AController* Exiting) override;

	void SetPlayerReady(APlayerController* ReadyPlayer, bool Value);

	// called by the client when it has properly set up the menu in the lobby level
	void PostClientReadyNotification(class ALobbyPlayerController* Notifier);
	
	UFUNCTION(Exec)
		void SetPlayTimeExec(int32 PlayTime);

private:
	const int NumTeams = 2;

	int32 DesiredPlayersToStart = 4;

	int32 DesiredPlayTime = -1;
	bool bOverrideDefaultPlayTime = false;

	UPROPERTY(EditDefaultsOnly)
		float CountdownToStart = 3.0f;

	TArray<class ALobbyPlayerController*> ConnectedPlayersControllers;

	void StartGame();
	FTimerHandle GameStartTimer;

	int32 ChooseTeam(class AHotStuffPlayerState* ForPlayerState) const;
};
