// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneController.h"
#include "Drone.h"
#include "HotStuffPlayerState.h"

#include "TeamBase.h"
#include "SpawnZoneDelimiter.h"
#include "Kismet/GameplayStatics.h"

void ADroneController::LookAround(float Val)
{
	if (!GetPawn())
		return;

	USceneComponent* Azimuth = GetPawn()->GetRootComponent()->GetChildComponent(0);
	Azimuth->AddLocalRotation(FRotator(0, -Val * CameraSpeed, 0).Quaternion());
}

void ADroneController::LookUpDown(float Val)
{
	if (!GetPawn())
		return;

	USceneComponent* Elevation = GetPawn()->GetRootComponent()->GetChildComponent(0)->GetChildComponent(0);

	FRotator NewRotator = Elevation->RelativeRotation + FRotator(Val * CameraSpeed, 0, 0);

	Elevation->AddLocalRotation(FRotator(-Val * CameraSpeed, 0, 0).Quaternion());

}

void ADroneController::ClientMarkKill_Implementation(const FString& KilledPlayerName, bool bFriendlyFire)
{
	FString Message = "Destroyed " + KilledPlayerName;

	if (bFriendlyFire)
	{
		Message.Append(" With friendly fire!");
	}

	ShowHUDMessage(Message);
}

void ADroneController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	auto HSPlayerState = Cast<AHotStuffPlayerState>(PlayerState);

	if (HSPlayerState)
	{
		UpdateBases();
		//UpdateSpawnDelimiters();

		auto HSPawn = Cast<ADrone>(GetPawn());
		if (!ensure(HSPawn)) return;

		HSPawn->UpdateName();
		HSPawn->UpdateMeshColor();
		HSPawn->UpdateCollisionChannel();
	}
}
// Spawn HUD for the client
void ADroneController::AcknowledgePossession(APawn * P)
{
	Super::AcknowledgePossession(P);
	UE_LOG(LogTemp, Log, TEXT("ACK"))
	if (HUD)
	{
		UE_LOG(LogTemp, Log, TEXT("ACK and Widgets"))
		SetupWidgetsPawn(P);
	}
}
// Spawn HUD for the listen server
void ADroneController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);
	UE_LOG(LogTemp, Log, TEXT("POSSESS"))
	if (HUD)
	{
		UE_LOG(LogTemp, Log, TEXT("POSSESS and Widgets"))
		SetupWidgetsPawn(InPawn);
	}
}

void ADroneController::UpdateBases()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamBase::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		Cast<ATeamBase>(Actor)->UpdateMeshColor();
	}
}

/*void ADroneController::UpdateSpawnDelimiters()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnZoneDelimiter::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		Cast<ASpawnZoneDelimiter>(Actor)->UpdateMeshColor();
	}
}*/
