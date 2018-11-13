// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerRow.h"
#include "MainMenu.h"

void UServerRow::Setup(UMainMenu * Parent, uint32 Index)
{
	this->Parent = Parent;
	MyIndex = Index;
}

void UServerRow::SelectThisServer()
{
	Parent->SelectServerIndex(MyIndex);
}
