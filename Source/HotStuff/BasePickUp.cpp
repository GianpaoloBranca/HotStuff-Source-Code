// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePickUp.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABasePickUp::ABasePickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called every frame
void ABasePickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FQuat q = FQuat(-DeltaTime, -DeltaTime, -DeltaTime, 1);

	GetRootComponent()->AddLocalRotation(q);
}

