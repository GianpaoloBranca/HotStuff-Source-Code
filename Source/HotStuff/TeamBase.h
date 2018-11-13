// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "TeamBase.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API ATeamBase : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
		void UpdateMeshColor();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing=OnRep_TeamNumber)
		int32 TeamNumber;

	UFUNCTION()
		void OnRep_TeamNumber();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Setup")
		void SetupMeshColor(bool Enemy);

};
