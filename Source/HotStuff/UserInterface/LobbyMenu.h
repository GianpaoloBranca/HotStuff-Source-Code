// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyMenu.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API ULobbyMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	ULobbyMenu(const FObjectInitializer & ObjectInitializer);

	void RefreshPlayers();

	void AddPlayerProfile(FString Name, int Team);

	void SetPlayerReady(FString Name, bool Value);

	UFUNCTION(BlueprintImplementableEvent)
		void CanBeReady(bool Value);

	UFUNCTION(BlueprintImplementableEvent)
		void OnStartingMatch(float RemainingSeconds);

private:
	TSubclassOf<class UUserWidget> PlayerProfileClass;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* YourTeam;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* EnemyTeam;
	
	UFUNCTION(BlueprintPure)
		FString GenerateStartingMessage(float Seconds) const;
};
