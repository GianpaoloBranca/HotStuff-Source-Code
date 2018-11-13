// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "DroneController.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API ADroneController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
		void LookAround(float Val);

	UFUNCTION(BlueprintCallable, Category = "Camera")
		void LookUpDown(float Val);

	UFUNCTION(Client, Unreliable)
		void ClientMarkKill(const FString& KilledPlayerName, bool bFriendlyFire = false);

	void OnRep_PlayerState() override;

	void AcknowledgePossession(APawn * P) override;
	void Possess(APawn * InPawn) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetupWidgetsPawn(APawn * InPawn);

	UPROPERTY(BlueprintReadWrite)
		UUserWidget * HUD;

	UPROPERTY(BlueprintReadWrite)
		UUserWidget * Compass;

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void ShowHUDMessage(const FString& Msg);

private:
	float CameraSpeed = 2.0f;

	void UpdateBases();
	void UpdateSpawnDelimiters();
};
