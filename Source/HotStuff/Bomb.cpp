// Fill out your copyright notice in the Description page of Project Settings.

#include "Bomb.h"
#include "Net/UnrealNetwork.h"
#include "Drone.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

void ABomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// This actually takes care of replicating the Variable
	DOREPLIFETIME(ABomb, BombHp);
}

void ABomb::ApplyDamageOvertime()
{
	if (HasAuthority() && BombHp>0)
	{
		TakeDamage(DamageOvertime, FDamageEvent(), nullptr, nullptr);
	}
}

float ABomb::CalculateDamage(FVector ObjectPosition)
{
	FVector BombPosition = GetActorLocation();
	int Distance = (FVector::Distance(ObjectPosition, BombPosition)/1000)+1;

	float DamageToApply = (100/Distance) * 2;

	return DamageToApply;
}

FVector ABomb::CalculateBombSpeed(float Factor, float MaxSlowedVelocity)
{
	float Max = GetVelocity().Size();

	Max = FMath::Lerp(Max, MaxSlowedVelocity, Factor);

	FVector SlowedSpeed = GetVelocity();

	return SlowedSpeed.GetClampedToSize(0.0f, Max);
}

void ABomb::Server_ApplyDamageOvertime_Implementation()
{
	ApplyDamageOvertime();
}

bool ABomb::Server_ApplyDamageOvertime_Validate()
{
	return true;
}


float ABomb::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	OnRep_BombHp(Damage);
	if (BombHp == 0 && !Destroyed)
	{
		Destroyed = true;
		Explode();
		return Damage;
	}

	ControllerThatTrigger = (APlayerController*)EventInstigator;

	BombHp -= Damage;

	return Damage;
}