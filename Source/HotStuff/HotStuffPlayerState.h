// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HotStuffPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API AHotStuffPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	void CopyProperties(APlayerState * PlayerState) override;

	void SetTeamNumber(int32 TeamNumber);

	void ClientInitialize(AController * C) override;

	void OnRep_PlayerName() override;

	UFUNCTION()
		void OnRep_TeamNumber();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
		void ScoreDeath();

	UFUNCTION(BlueprintPure)
		int32 GetTeamNumber();

	UFUNCTION(BlueprintPure)
		int32 GetNumDeaths();

private:
	UPROPERTY(Transient, ReplicatedUsing=OnRep_TeamNumber)
		int32 TeamNumber;

	UPROPERTY(Transient, Replicated)
		int32 NumDeaths;
};
