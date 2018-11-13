// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnZoneDelimiter.generated.h"

UCLASS()
class HOTSTUFF_API ASpawnZoneDelimiter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnZoneDelimiter();

	UFUNCTION(BlueprintCallable)
		void UpdateMeshColor();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated)
		int32 TeamNumber;

	UFUNCTION(NetMulticast, Reliable)
		void MulticastOpenGate();

protected:

	UFUNCTION(BlueprintImplementableEvent)
		void OpenGate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Setup")
		void SetupMeshColor(bool Enemy);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class UBoxComponent* BlockingVolume;


};
