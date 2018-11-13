// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	FString HostUserName;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
};

/**
 * 
 */
UCLASS()
class HOTSTUFF_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainMenu(const FObjectInitializer & ObjectInitializer);

	void SetMenuInterface(IMenuInterface * MenuInterface);

	void SetServerList(TArray<FServerData> ServerNames);

	void SelectServerIndex(uint32 Index);
	
private:
	IMenuInterface * MenuInterface = nullptr;

	UFUNCTION(BlueprintCallable)
		void HostServer(FString ServerName, int32 NumPlayers);	
	
	UFUNCTION(BlueprintCallable)
		void JoinServer(FString UserName);

	UFUNCTION(BlueprintCallable)
		void OpenHostMenu();

	UFUNCTION(BlueprintCallable)
		void OpenJoinMenu();

	UFUNCTION(BlueprintCallable)
		void OpenMainMenu();

	UFUNCTION(BlueprintCallable)
		void OpenAboutMenu();

	UFUNCTION(BlueprintCallable)
		void QuitGame();

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
		class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget* AboutMenu;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ServerList;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* ServerListSwitcher;

	UPROPERTY(meta = (BindWidget))
		class UWidget* LoadingCircle;

	TSubclassOf<class UUserWidget> ServerRowClass;
	TOptional<uint32> SelectedServerIndex;

	void UpdateChildren();
};
