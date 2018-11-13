// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Bomb.generated.h"

class APlayerController;
class UStaticMeshComponent;
class ADrone;

/**
 * 
 */
UCLASS()
class HOTSTUFF_API ABomb : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
		bool IsAttached;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Destroyed=false;

	//This is an event implemented into the Drone_BP
	UFUNCTION(BlueprintImplementableEvent, Category = "Explode")
		void Explode();

	UFUNCTION(BlueprintImplementableEvent, Category = "BombMaterial")
		void OnRep_BombHp(int32 Damage);

	void ApplyDamageOvertime();

	UFUNCTION(BlueprintImplementableEvent, Category = "Drone")
	void RemoveFromDrone();

	UFUNCTION(BlueprintCallable, Category = "Damage", Server, Unreliable, WithValidation)
		void Server_ApplyDamageOvertime();

	UFUNCTION(BlueprintCallable, Category = "Damage")
		float CalculateDamage(FVector ObjectPosition);


	UFUNCTION(BlueprintCallable, Category = "Slow")
		FVector CalculateBombSpeed(float Factor, float MaxSlowedVelocity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_BombHp)
		int BombHp = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageOvertime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, replicated)
		APlayerController * ControllerThatTrigger;

private:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

};
