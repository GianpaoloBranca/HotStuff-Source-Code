// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "WheeledVehicle.h"
#include "HotStuffPlayerState.h"
#include "Engine/World.h"
#include "HotStuffGameState.h"
#include "Drone.generated.h"

class UWheeledVehicleMovementComponent;
class UGameplayStatics;
class AStaticMeshActor;
class ABullet;
class USpringArmComponent;
class UStaticMeshComponent;
class OutputDeviceNull;
class ABomb;

/**
 *
 */
UCLASS()
class HOTSTUFF_API ADrone : public AWheeledVehicle
{
	GENERATED_BODY()

public:

	ADrone();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
		int32 BoostAmount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values", meta = (ClampMin = "0.0", ClampMax = "105.0", UIMin = "0.0", UIMax = "105.0"), ReplicatedUsing=OnRep_HealthPoints )
		int32 HealthPoints = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Bomb")
	ABomb* BombRef;

	//Input functions
	//Move return true if the drone is going forward
	//false otherwise
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void Move(float Val);

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void Steer(float Val);
	
	UFUNCTION(BlueprintCallable)
		void UseBoost();

	UFUNCTION(BlueprintCallable, Category = "Boost", Server, Unreliable, WithValidation)
		void Server_UseBoost();

	UFUNCTION(BlueprintCallable, Category = "Fire", Server, Reliable, WithValidation)
		void Server_StartFire();

	UFUNCTION(BlueprintCallable, Category = "Fire", Server, Reliable, WithValidation)
		void Server_StopFire();

	UFUNCTION(BlueprintCallable)
		void ReleaseBoost();

	UFUNCTION(BlueprintCallable, Category = "Boost", Server, Unreliable, WithValidation)
		void Server_ReleaseBoost();

	void ReleaseBomb();

	UFUNCTION(BlueprintCallable, Category = "Bomb", Server, Unreliable, WithValidation)
		void Server_ReleaseBomb();

	//This is an event implemented into the Drone_BP
	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
		void Death();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
		void UpdateForceBar(const int32 &Force);

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void ApplyDamageOvertime();

		void RestoreHp();

	UFUNCTION(BlueprintCallable, Category = "HP", Server, Unreliable, WithValidation)
		void Server_RestoreHp();

	void SetUpForLaunch(USpringArmComponent* BombLaunchPoint);

	UFUNCTION(BlueprintCallable, Category = "Launch", Server, Unreliable, WithValidation)
		void Server_SetUpForLaunch(USpringArmComponent* BombLaunchPoint);

	void LaunchBomb();

	UFUNCTION(BlueprintCallable, Category = "Launch", Server, Unreliable, WithValidation)
		void Server_LaunchBomb();

	UFUNCTION(BlueprintCallable, Category = "Launch", Server, Unreliable, WithValidation)
		void Server_ChargingLaunchForce();

	void ChargingLaunchForce();

	UFUNCTION(BlueprintCallable, Category = "Boost")
		void RestoreBoost();

	UFUNCTION(BlueprintCallable, Category = "Boost")
		void ConsumeBoostAmount();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, replicated)
		bool IsHoldingTheBomb = false;

	UFUNCTION(BlueprintCallable, Category = "Slow")
		FVector CalculateSlowedSpeed(float Factor, float MaxSlowedVelocity);

	UFUNCTION(BlueprintCallable, Category = "Check")
		void CheckIfUpsideDown();

	UFUNCTION(BlueprintImplementableEvent, Category = "Adjustment")
		void AdjustRotation();

	void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
		void UpdateMeshColor();

	UFUNCTION(BlueprintCallable)
		void UpdateName();

	UFUNCTION(BlueprintCallable)
		void UpdateCollisionChannel();

	//This variable disable player input for 3 seconds, before the drone's explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, replicated)
		bool Dead = false;
	//Stop damage overtime function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, replicated)
		bool Slowed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, replicated)
		float TimeBetweenShoots = 0.2f;

	UPROPERTY(EditAnywhere, Replicated)
		float ConsumeBoostRate = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool BoostUsed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, replicatedUsing = OnRep_LaunchForce)
		float LaunchForce = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LastDamageTimestamp = 0.0f;

	void PossessedBy(AController * NewController) override;

	//Bullet class
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AStaticMeshActor> BulletClass;

	UPROPERTY(BlueprintReadOnly)
		float FadingDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxLaunchForce = 500000.0f;

	void RestoreDefaultBoost();
	void RestoreDefaultFire();

	void ActivateRapidFire(float RapidTimeBetweenShots);
	void ActivateInfiniteBoost();

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Setup")
		void SetupCollisionChannel(int32 TeamNumber);

	UFUNCTION(BlueprintImplementableEvent, Category = "Setup")
		void SetupInfoWidget(const FString& Name);

	UFUNCTION(BlueprintImplementableEvent, Category = "Setup")
		void SetupMeshColor(bool Enemy);

private:

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	void Fire();

	int32 PreviousHealth = 100;

	UPROPERTY(EditAnywhere)
		float FadingDamageRate = 10.0f;

	float DefaultTimeBetweenShots;
	float DefaultConsumeBoostRate;

	UFUNCTION()
		void OnRep_HealthPoints();

	UFUNCTION()
		void OnRep_LaunchForce();

	UFUNCTION(BlueprintCallable)
		void DestroyDrone();

	APlayerController* LastControllerThatFired;

	APlayerState* GetLocalPlayerState();

	float TimeElapsedLastBoost = 0.0f;
	float TimeElapsedLastBullet = TimeBetweenShoots;

	FTimerHandle ContinuousFireTimer;
};
