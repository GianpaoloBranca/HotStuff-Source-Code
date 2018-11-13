// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"

void ABullet::SetControllerThatFired(APlayerController* Controller)
{
	ControllerThatFired = Controller;
}

APlayerController* ABullet::GetControllerThatFired()
{
	return ControllerThatFired;
}
