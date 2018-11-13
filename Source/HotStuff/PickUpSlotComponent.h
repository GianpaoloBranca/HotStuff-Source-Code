// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickUpSlotComponent.generated.h"

class ADrone;
class UPickUpsWidget;

UENUM(BlueprintType)
enum class EPickUp : uint8
{
	None			UMETA(DisplayName = "None"),
	InfiniteBoost	UMETA(DisplayName = "Infinite Boost"),
	RapidFire		UMETA(DisplayName = "Rapid Fire"),
};

UENUM()
enum class ESlotStatus : uint8
{
	Empty,
	Filled,
	InUse
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOTSTUFF_API UPickUpSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPickUpSlotComponent();

	UFUNCTION(BlueprintCallable)
		void Init(ADrone* Drone);

	UFUNCTION(BlueprintCallable)
		void InitWidget(UPickUpsWidget* Widget);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Unreliable)
		void ServerRequestUse();

	UFUNCTION(BlueprintCallable)
		void Fill();

	UFUNCTION(BlueprintPure)
		bool IsEmpty();

private:

	void Reset();
	void Use();

	UFUNCTION(Client, Reliable)
		void ClientDisplayFilledSlot(EPickUp PickUp);
	UFUNCTION(Client, Reliable)
		void ClientDisplaySlotInUse(float Duration);
	UFUNCTION(Client, Reliable)
		void ClientDisplayEmptySlot();

	FTimerHandle InUseTimer;

	UPROPERTY(EditAnywhere)
		float InfiniteBoostDuration = 10.0f;
	UPROPERTY(EditAnywhere)
		float RapidFireDuration = 10.0f;
	UPROPERTY(EditAnywhere)
		float RapidFireTimeBetweenShots = 0.1f;

	EPickUp PickUp = EPickUp::None;
	ESlotStatus SlotStatus = ESlotStatus::Empty;

	ADrone* Target = nullptr;
	UPickUpsWidget* Widget = nullptr;
};
