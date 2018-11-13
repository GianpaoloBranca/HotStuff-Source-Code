// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Bullet.generated.h"

/**
 * Everytime we create a bullet, we must also link it to the drone that fired
 */
UCLASS()
class HOTSTUFF_API ABullet : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	void SetControllerThatFired(APlayerController* Controller);

	UFUNCTION(BlueprintCallable, Category = "Controller")
		APlayerController* GetControllerThatFired();

private:
	APlayerController * ControllerThatFired;



	
	
};
