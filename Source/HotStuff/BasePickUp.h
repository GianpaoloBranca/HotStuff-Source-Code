// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickUp.generated.h"

UCLASS()
class HOTSTUFF_API ABasePickUp : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABasePickUp();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
