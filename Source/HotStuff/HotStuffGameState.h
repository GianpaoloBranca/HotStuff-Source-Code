// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HotStuffGameState.generated.h"

/**
 * 
 */

UCLASS()
class HOTSTUFF_API AHotStuffGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
		void UpdateScore(int32 Points, int32 TeamNum);

	UFUNCTION(BlueprintPure)
		int32 GetWinningTeam();

	UFUNCTION(BlueprintPure)
		TArray<int32> GetTeamScores();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastGoalNotification(int32 ScoringTeam);

	UFUNCTION(NetMulticast, Reliable)
		void GameOver(int32 WinnerTeam);

	UPROPERTY(BlueprintReadWrite)
		class UInGameMatchInfo* InGameMatchInfoWidget = nullptr;

	UPROPERTY(BlueprintReadWrite, Replicated)
		FName MatchState;

	void StartCountDown(int32 Seconds);

private:
	FTimerHandle CountDownTimer;

	void CountDown();

	UFUNCTION()
		void OnRep_Score();

	UFUNCTION()
		void OnRep_TimeCounter();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_TimeCounter)
		int32 RemainingTime;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Score)
		TArray<int32> TeamScores;

	UFUNCTION(BlueprintPure)
		TArray<APlayerState*> GetNoobsPlayerState(int32 WinningTeam) const;
};
