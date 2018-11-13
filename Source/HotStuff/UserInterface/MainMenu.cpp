// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "UObject/ConstructorHelpers.h"

#include "ServerRow.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/UserInterface/BP_ServerRow"));

	if (ensure(ServerRowBPClass.Class))
	{
		ServerRowClass = ServerRowBPClass.Class;
	}
}

void UMainMenu::SetMenuInterface(IMenuInterface * MenuInterface)
{
	this->MenuInterface = MenuInterface;
}

void UMainMenu::HostServer(FString ServerName, int32 NumPlayers)
{
	if (!ensure(MenuInterface)) return;

	MenuInterface->Host(ServerName, NumPlayers);
}

void UMainMenu::JoinServer(FString UserName)
{
	if (SelectedServerIndex.IsSet() && MenuInterface)
	{		
		MenuInterface->Join(UserName, SelectedServerIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index Not Set"))
	}

}

void UMainMenu::OpenHostMenu()
{
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuInterface)) return;

	MenuSwitcher->SetActiveWidget(JoinMenu);
	ServerListSwitcher->SetActiveWidget(LoadingCircle);
	MenuInterface->RefreshServerList();
}

void UMainMenu::OpenMainMenu()
{
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenAboutMenu()
{
	MenuSwitcher->SetActiveWidget(AboutMenu);
}

void UMainMenu::QuitGame()
{
	if (!ensure(MenuInterface)) return;
	
	MenuInterface->QuitGame();
}

void UMainMenu::SetServerList(TArray<FServerData> ServerDatas)
{	
	UWorld* World = GetWorld();
	if (!ensure(World)) return;

	ServerListSwitcher->SetActiveWidget(ServerList);

	ServerList->ClearChildren();

	uint32 i = 0;
	for (auto ServerData : ServerDatas)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row)) return;

		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->UserName->SetText(FText::FromString(ServerData.HostUserName));

		FString Fraction = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->PlayersNumber->SetText(FText::FromString(Fraction));

		Row->Setup(this, i);

		++i;
		ServerList->AddChild(Row);
	}

	if (ServerList->HasAnyChildren())
	{
		auto ServerRow = Cast<UServerRow>(ServerList->GetChildAt(0));
		ServerRow->RowButton->SetUserFocus(GetOwningPlayer());
	}
}

void UMainMenu::SelectServerIndex(uint32 Index)
{
	SelectedServerIndex = Index;
	UpdateChildren();
}


void UMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Row)
		{
			Row->bSelected = (SelectedServerIndex.IsSet() && SelectedServerIndex.GetValue() == i);
		}
	}
}
