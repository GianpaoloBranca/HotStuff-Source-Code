// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class HOTSTUFF_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* RowButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* UserName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PlayersNumber;

	UPROPERTY(BlueprintReadOnly)
		bool bSelected = false;

	void Setup(class UMainMenu* Parent, uint32 Index);

private:

	UFUNCTION(BlueprintCallable)
		void SelectThisServer();

	class UMainMenu* Parent = nullptr;
	uint32 MyIndex;
};
