// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickUpSlotComponent.h"

#include "PickUpsWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API UPickUpsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintImplementableEvent)
		void FillSlot(EPickUp PickUp);
	
	UFUNCTION(BlueprintImplementableEvent)
		void EmptySlot();

	UFUNCTION(BlueprintImplementableEvent)
		void DisplayUsage(float Duration);
};
