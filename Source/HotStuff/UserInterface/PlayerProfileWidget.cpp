// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerProfileWidget.h"
#include "Components/TextBlock.h"

const static FText NOT_READY_TEXT = FText::FromString("");
const static FText READY_TEXT = FText::FromString("Ready!");

void UPlayerProfileWidget::Setup(FString PlayerName)
{
	this->Name = PlayerName;
	this->PlayerName->SetText(FText::FromString(PlayerName));
	this->ReadyText->SetText(NOT_READY_TEXT);
}

FString UPlayerProfileWidget::GetName()
{
	return Name;
}

void UPlayerProfileWidget::SetReady(bool Value)
{
	if (Value)
	{
		this->ReadyText->SetText(READY_TEXT);
	}
	else
	{
		this->ReadyText->SetText(FText());
	}
}
