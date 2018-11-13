// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUpSlotComponent.h"
#include "Drone.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UserInterface/PickUpsWidget.h"


UPickUpSlotComponent::UPickUpSlotComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickUpSlotComponent::Init(ADrone * Target)
{
	this->Target = Target;
	
	PickUp = EPickUp::None;
	SlotStatus = ESlotStatus::Empty;
}

void UPickUpSlotComponent::InitWidget(UPickUpsWidget * Widget)
{
	this->Widget = Widget;

	ClientDisplayEmptySlot();
}

void UPickUpSlotComponent::ServerRequestUse_Implementation()
{
	if (SlotStatus == ESlotStatus::Filled)
	{
		Use();
	}
}

bool UPickUpSlotComponent::ServerRequestUse_Validate()
{
	return true;
}

void UPickUpSlotComponent::Use()
{
	if (!ensure(Target)) return;

	UE_LOG(LogTemp, Log, TEXT("Activated a power up"))

	SlotStatus = ESlotStatus::InUse;

	switch (PickUp)
	{
	case EPickUp::None:
		UE_LOG(LogTemp, Warning, TEXT("Attempted to use an empty slot"))
		Reset();
		break;
	case EPickUp::InfiniteBoost:
		Target->ActivateInfiniteBoost();
		GetWorld()->GetTimerManager().SetTimer(InUseTimer, this, &UPickUpSlotComponent::Reset, InfiniteBoostDuration);
		ClientDisplaySlotInUse(InfiniteBoostDuration);
		break;
	case EPickUp::RapidFire:
		Target->ActivateRapidFire(RapidFireTimeBetweenShots);
		GetWorld()->GetTimerManager().SetTimer(InUseTimer, this, &UPickUpSlotComponent::Reset, RapidFireDuration);
		ClientDisplaySlotInUse(RapidFireDuration);
		break;
	default:
		break;
	}
}

void UPickUpSlotComponent::ClientDisplayFilledSlot_Implementation(EPickUp PickUp)
{
	if (!ensure(Widget)) return;
	Widget->FillSlot(PickUp);
}

void UPickUpSlotComponent::ClientDisplaySlotInUse_Implementation(float Duration)
{
	if (!ensure(Widget)) return;
	Widget->DisplayUsage(Duration);
}

void UPickUpSlotComponent::ClientDisplayEmptySlot_Implementation()
{
	if (!ensure(Widget)) return;
	Widget->EmptySlot();
}

void UPickUpSlotComponent::Fill()
{
	PickUp = static_cast<EPickUp>(FMath::RandRange(1, 2));
	SlotStatus = ESlotStatus::Filled;

	ClientDisplayFilledSlot(PickUp);
}

bool UPickUpSlotComponent::IsEmpty()
{
	return SlotStatus == ESlotStatus::Empty;
}

void UPickUpSlotComponent::Reset()
{
	if (!ensure(Target)) return;

	switch (PickUp)
	{
	case EPickUp::None:
		break;
	case EPickUp::InfiniteBoost:
		UE_LOG(LogTemp, Log, TEXT("Boost going to normal"))
		Target->RestoreDefaultBoost();
		break;
	case EPickUp::RapidFire:
		UE_LOG(LogTemp, Log, TEXT("Fire rate going to normal"))
		Target->RestoreDefaultFire();
		break;
	default:
		break;
	}

	PickUp = EPickUp::None;
	SlotStatus = ESlotStatus::Empty;

	ClientDisplayEmptySlot();
}

