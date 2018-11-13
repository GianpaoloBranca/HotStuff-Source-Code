// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMatchInfo.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API UInGameMatchInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateScores(int32 YourScore, int32 EnemyScore);

	UFUNCTION(BlueprintImplementableEvent)
		void ShowGoalMessage(bool EnemyGoal);

	UFUNCTION(BlueprintImplementableEvent)
		void ShowMatchOutcome(bool Victory);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowMessage(const FString& Message, float Duration = -1.0f);

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateTimerDisplay(int32 RemainingTime);
	
};
