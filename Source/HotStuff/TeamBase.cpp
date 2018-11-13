// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamBase.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "HotStuffPlayerState.h"

void ATeamBase::UpdateMeshColor()
{
	auto MyPlayerController = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
	if (!ensure(MyPlayerController)) return;

	auto LocalHSPlayerState = Cast<AHotStuffPlayerState>(MyPlayerController->PlayerState);
	if (LocalHSPlayerState)
	{
		SetupMeshColor(LocalHSPlayerState->GetTeamNumber() != TeamNumber);
	}
}

void ATeamBase::OnRep_TeamNumber()
{
	UpdateMeshColor();
}

void ATeamBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamBase, TeamNumber)
}