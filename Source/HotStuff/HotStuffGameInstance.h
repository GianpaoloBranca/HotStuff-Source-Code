// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UserInterface/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"

#include "HotStuffGameInstance.generated.h"

class UMainMenu;
/**
 * 
 */
UCLASS()
class HOTSTUFF_API UHotStuffGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:

	void Init();

	void Host(FString ServerName, int32 NumPlayers) override;

	void Join(FString UserName, uint32 Index) override;

	void StartSession();

	void RefreshServerList() override;

	// this is not exposed in blueprint by default
	UFUNCTION(BlueprintPure)
		APlayerController* GetMyPlayerController();

	UFUNCTION(BlueprintCallable)
		void QuitToMainMenu() override;

	UFUNCTION(BlueprintCallable)
		void QuitGame() override;

	UFUNCTION(BlueprintCallable)
		void SetMainMenuWidget(UMainMenu* Menu);

	UFUNCTION(Exec)
		void JoinExec(FString Address);
		
private:
	UMainMenu* MainMenuWidget = nullptr;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FString DesiredServerName;
	int32 DesiredNumPlayers;

	UPROPERTY(EditDefaultsOnly)
		int32 DefaultNumPlayers = 4;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
