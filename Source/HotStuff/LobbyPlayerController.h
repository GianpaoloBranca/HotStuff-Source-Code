// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"
/**
 * 
 */
UCLASS()
class HOTSTUFF_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()	

public:

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable)
		void ServerNotifyMenuInitialized();

	UFUNCTION(BlueprintCallable, Server, WithValidation, Unreliable)
		void ServerReady(bool Value);

	UFUNCTION(Client, Reliable)
		void ClientRefreshPlayersList();

	UFUNCTION(Client, Reliable)
		void ClientSetPlayerReady(const FString& PlayerName, bool Value);

	UFUNCTION(Client, Reliable)
		void ClientCanBeReady(bool Value);

	UFUNCTION(Client, Reliable)
		void ClientStartingMatch(float RemainingSeconds);

	UPROPERTY(BlueprintReadWrite)
		class ULobbyMenu* LobbyMenuWidget = nullptr;

	bool bReady = false;
};
