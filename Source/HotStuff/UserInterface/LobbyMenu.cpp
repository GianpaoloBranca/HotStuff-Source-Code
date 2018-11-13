// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyMenu.h"
#include "Engine.h"
#include "Components/HorizontalBox.h"
#include "HotStuffPlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "UserInterface/PlayerProfileWidget.h"

ULobbyMenu::ULobbyMenu(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> PlayerProfileBPClass(TEXT("/Game/UserInterface/BP_PlayerProfile"));
	if (!ensure(PlayerProfileBPClass.Class)) return;
	
	PlayerProfileClass = PlayerProfileBPClass.Class;
}

void ULobbyMenu::RefreshPlayers()
{
	UE_LOG(LogTemp, Log, TEXT("Refreshing Players"))

	auto GameState = GetWorld()->GetGameState();
	if (!GameState) return;

	UE_LOG(LogTemp, Log, TEXT("%d Players in GameState"), GameState->PlayerArray.Num())

	YourTeam->ClearChildren();
	EnemyTeam->ClearChildren();

	for (auto Player : GameState->PlayerArray)
	{
		auto HotStuffPlayer = Cast<AHotStuffPlayerState>(Player);
		if (!ensure(HotStuffPlayer)) return;

		AddPlayerProfile(HotStuffPlayer->GetPlayerName(), HotStuffPlayer->GetTeamNumber());
	}
}

void ULobbyMenu::AddPlayerProfile(FString Name, int Team)
{
	auto PlayerProfile = CreateWidget<UPlayerProfileWidget>(GetWorld(), PlayerProfileClass);
	if (!ensure(PlayerProfile)) return;
	
	auto YourState = Cast<AHotStuffPlayerState>(GetOwningPlayerState());
	if (!YourState) return;

	PlayerProfile->Setup(Name);

	if (Team == YourState->GetTeamNumber())
	{
		YourTeam->AddChild(PlayerProfile);
	}
	else
	{
		EnemyTeam->AddChild(PlayerProfile);
	}
}

void ULobbyMenu::SetPlayerReady(FString Name, bool Value)
{
	for (int32 i = 0; i < YourTeam->GetChildrenCount(); ++i)
	{
		auto PlayerProfile = Cast<UPlayerProfileWidget>(YourTeam->GetChildAt(i));
		if (!ensure(PlayerProfile)) return;

		if (PlayerProfile->GetName() == Name)
		{
			PlayerProfile->SetReady(Value);
			return;
		}
	}
	
	for (int32 i = 0; i < EnemyTeam->GetChildrenCount(); ++i)
	{
		auto PlayerProfile = Cast<UPlayerProfileWidget>(EnemyTeam->GetChildAt(i));
		if (!ensure(PlayerProfile)) return;

		if (PlayerProfile->GetName() == Name)
		{
			PlayerProfile->SetReady(Value);
			return;
		}
	}
}

FString ULobbyMenu::GenerateStartingMessage(float Seconds) const
{
	return FString::Printf(TEXT("Starting in %2.1f seconds"), Seconds);
}