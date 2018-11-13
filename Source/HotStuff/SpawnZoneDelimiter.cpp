// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnZoneDelimiter.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "HotStuffPlayerState.h"

// Sets default values
ASpawnZoneDelimiter::ASpawnZoneDelimiter()
{
	PrimaryActorTick.bCanEverTick = true;

	BlockingVolume = CreateDefaultSubobject<UBoxComponent>(FName("Blocking Volume"));
	if (!ensure(BlockingVolume != nullptr)) return;

	RootComponent = BlockingVolume;
}

void ASpawnZoneDelimiter::UpdateMeshColor()
{
	auto MyPlayerController = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
	if (!ensure(MyPlayerController)) return;

	auto LocalHSPlayerState = Cast<AHotStuffPlayerState>(MyPlayerController->PlayerState);
	if (LocalHSPlayerState)
	{
		SetupMeshColor(LocalHSPlayerState->GetTeamNumber() != TeamNumber);
	}
}

void ASpawnZoneDelimiter::MulticastOpenGate_Implementation()
{
	UpdateMeshColor();
	OpenGate();
	UE_LOG(LogTemp, Warning, TEXT("Open Gates!"))
}

void ASpawnZoneDelimiter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpawnZoneDelimiter, TeamNumber)
}