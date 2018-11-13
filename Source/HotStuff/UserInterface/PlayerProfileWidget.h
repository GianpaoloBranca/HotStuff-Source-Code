// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerProfileWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API UPlayerProfileWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Setup(FString PlayerName);

	FString GetName();

	void SetReady(bool Value);

private:
	FString Name;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock * PlayerName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock * ReadyText;
	
};
