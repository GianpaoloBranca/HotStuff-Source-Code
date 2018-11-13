// Fill out your copyright notice in the Description page of Project Settings.

#include "Drone.h"
#include "HotStuffGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Bullet.h"
#include "Bomb.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Runtime/Core/Public/Misc/OutputDeviceNull.h"
#include "WheeledVehicleMovementComponent.h"
#include "DroneController.h"

#include "TimerManager.h"

ADrone::ADrone()
{
	Dead = false;
}

void ADrone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// This actually takes care of replicating the Variable
	DOREPLIFETIME(ADrone, HealthPoints);
	DOREPLIFETIME(ADrone, Slowed);
	DOREPLIFETIME(ADrone, LaunchForce);
	DOREPLIFETIME(ADrone, IsHoldingTheBomb);
	DOREPLIFETIME(ADrone, Dead);
	DOREPLIFETIME(ADrone, ConsumeBoostRate);
	DOREPLIFETIME(ADrone, TimeBetweenShoots);
}

void ADrone::BeginPlay()
{
	Super::BeginPlay();

	DefaultTimeBetweenShots = TimeBetweenShoots;
	DefaultConsumeBoostRate = ConsumeBoostRate;
}

void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FadingDamage > 0)
	{
		FadingDamage -= DeltaTime * FadingDamageRate;
	}
}



void ADrone::Move(float Val)
{
	if (!Dead)
	{
		GetVehicleMovement()->SetThrottleInput(Val);

		float ForwardSpeed = GetVehicleMovementComponent()->GetForwardSpeed();

		USceneComponent* Azimuth = GetRootComponent()->GetChildComponent(0);

		float ThresHold = -10;

		//Update camera
		if (ForwardSpeed >= ThresHold)
		{
			Azimuth->SetRelativeRotation(FQuat(0, 0, 0, 0));
		}
		else if (ForwardSpeed < ThresHold && Val < 0.0f)
		{
			Azimuth->SetRelativeRotation(FQuat(0, 0, 180, 0));
		}
	}
}

void ADrone::Steer(float Val)
{
	if (!Dead)
		GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ADrone::UseBoost()
{
	if (!Dead)
	{
		//Comp n. 2 is the nitro into the drone bp
		USceneComponent* NitroComp = GetRootComponent()->GetChildComponent(2);
		NitroComp->SetActive(true);
	}
}

void ADrone::Server_UseBoost_Implementation()
{
	if (HasAuthority())
	{
		UseBoost();
	}
}

bool ADrone::Server_UseBoost_Validate()
{
	return true;
}

void ADrone::ReleaseBoost()
{
	if (!Dead)
	{
		//Comp n. 2 is the nitro into the drone bp
		USceneComponent* NitroComp = GetRootComponent()->GetChildComponent(2);
		NitroComp->SetActive(false);
	}
}

void ADrone::ReleaseBomb()
{
	if (IsHoldingTheBomb && BombRef)
	{
		BombRef->GetStaticMeshComponent()->SetSimulatePhysics(true);
		FDetachmentTransformRules Rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld,true);
		BombRef->DetachFromActor(Rules);
		IsHoldingTheBomb = false;
		BombRef->IsAttached = false;
	}
}

void ADrone::Server_ReleaseBomb_Implementation()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Display, TEXT("Bomb released"))
		ReleaseBomb();
	}
}

bool ADrone::Server_ReleaseBomb_Validate()
{
	return true;
}

void ADrone::Server_ReleaseBoost_Implementation()
{
	ReleaseBoost();
}

bool ADrone::Server_ReleaseBoost_Validate()
{
	return true;
}

void ADrone::ConsumeBoostAmount()
{
	if (!Dead)
	{
		TimeElapsedLastBoost = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		if (BoostAmount > 0)
		{
			//Boost
			BoostAmount -= ConsumeBoostRate;
			BoostUsed = true;
		}
		else
		{
			//No more boost
			BoostUsed = false;
			Server_ReleaseBoost();
		}
	}
	else
	{
		BoostUsed = false;
	}
}

void ADrone::ApplyDamageOvertime()
{
	TakeDamage(5.0f, FDamageEvent(), nullptr, nullptr);
}

void ADrone::RestoreHp()
{
	float Delta = GetWorld()->GetRealTimeSeconds() - LastDamageTimestamp;
	if (!Dead && Delta >= 5.0f && HealthPoints < 100)
		HealthPoints++;
}



void ADrone::SetUpForLaunch(USpringArmComponent* BombLaunchPoint)
{
	FDetachmentTransformRules DetachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	BombRef->DetachFromActor(DetachRules);
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
	BombRef->AttachToComponent(BombLaunchPoint, Rules, FName(""));
	FTransform BombTransform = FTransform(BombRef->GetActorQuat(), FVector(0, 0, 0), FVector(1, 1, 1));
	BombRef->SetActorRelativeTransform(BombTransform);
}

void ADrone::LaunchBomb()
{
	//Bombhp==-1 -> bomba piantata
	if (BombRef && IsHoldingTheBomb && BombRef->BombHp != -1)
	{
		BombRef->GetStaticMeshComponent()->SetSimulatePhysics(true);
		FDetachmentTransformRules Rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		BombRef->DetachFromActor(Rules);
		UPrimitiveComponent* BombPrimitive = Cast<UPrimitiveComponent>(BombRef->GetRootComponent());
		FVector ForceVector = GetActorForwardVector();
		ForceVector.Z = 0;
		ForceVector.Normalize();
		ForceVector *= LaunchForce;
		BombPrimitive->AddForce(ForceVector, FName(""), true);
		BombRef->IsAttached = false;
	}
	IsHoldingTheBomb = false;
	BombRef = nullptr;
	LaunchForce = 0;
}

void ADrone::Server_LaunchBomb_Implementation()
{
	if (HasAuthority() && IsHoldingTheBomb)
	{
		LaunchBomb();
	}
	else
	{
		LaunchForce = 0.0f;
		BombRef = nullptr;
	}
}

bool ADrone::Server_LaunchBomb_Validate()
{
	return true;
}

void ADrone::Server_SetUpForLaunch_Implementation(USpringArmComponent* BombLaunchPoint)
{
	if (HasAuthority() && IsHoldingTheBomb)
	{
		SetUpForLaunch(BombLaunchPoint);
	}
}

bool ADrone::Server_SetUpForLaunch_Validate(USpringArmComponent* BombLaunchPoint)
{
	return true;
}

void ADrone::Server_RestoreHp_Implementation()
{
	if (!Dead)
	{
		RestoreHp();
	}
}

bool ADrone::Server_RestoreHp_Validate()
{
	return true;
}

void ADrone::RestoreBoost()
{
	if (!Dead)
	{
		float Delta = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - TimeElapsedLastBoost;
		if (BoostAmount < 100 && Delta >= 2.0f)
			BoostAmount += 2;
	}
}

void ADrone::Fire()
{
	if (HasAuthority())
	{
		float CurrentTime = GetWorld()->GetRealTimeSeconds();
		bool Condition = FMath::IsNearlyEqual(FMath::Abs(CurrentTime - TimeElapsedLastBullet),TimeBetweenShoots,0.01f) 
			|| FMath::Abs(CurrentTime - TimeElapsedLastBullet) > TimeBetweenShoots;
		
		if (BulletClass && Condition)
		{
			TimeElapsedLastBullet = CurrentTime;
			FVector CannonHole = GetRootComponent()->GetSocketLocation(FName("Cannon"));
			FRotator DirectionRotator = GetRootComponent()->GetSocketRotation(FName("Cannon"));
			FActorSpawnParameters SpawnInfo;
			ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, CannonHole, DirectionRotator, SpawnInfo);
			APlayerController* DroneController = Cast<APlayerController>(GetController());

			if (Bullet && DroneController)
				Bullet->SetControllerThatFired(DroneController);
		}
	}

}

bool ADrone::Server_StartFire_Validate()
{
	return true;
}

void ADrone::Server_StartFire_Implementation()
{
	if (!Dead)
	{
		Fire();
		GetWorldTimerManager().SetTimer(ContinuousFireTimer, this, &ADrone::Fire, TimeBetweenShoots, true);
	}
}

bool ADrone::Server_StopFire_Validate()
{
	return true;
}

void ADrone::Server_StopFire_Implementation()
{
	GetWorldTimerManager().ClearTimer(ContinuousFireTimer);
}

void ADrone::ChargingLaunchForce()
{
	if (LaunchForce < MaxLaunchForce)
	{
		LaunchForce += 25000.0f;
		OnRep_LaunchForce();
	}
}

void ADrone::Server_ChargingLaunchForce_Implementation()
{
	if (HasAuthority() && IsHoldingTheBomb)
	{
		ChargingLaunchForce();
	}
}


bool ADrone::Server_ChargingLaunchForce_Validate()
{
	return true;
}


FVector ADrone::CalculateSlowedSpeed(float Factor, float MaxSlowedVelocity)
{
	float Max = GetVelocity().Size();

	Max = FMath::Lerp(Max, MaxSlowedVelocity, Factor);

	FVector SlowedSpeed = GetVelocity();

	return SlowedSpeed.GetClampedToSize(0.0f, Max);
}

void ADrone::CheckIfUpsideDown()
{
	FRotator MeshRotator = GetActorRotation();

	bool C0 = 135.0f >= FMath::Abs(MeshRotator.Roll) && FMath::Abs(MeshRotator.Roll) >= 45.0f;

	bool C1 = -45.0f >= FMath::Abs(MeshRotator.Pitch) || FMath::Abs(MeshRotator.Pitch) >= 45.0f;

	bool C2 = -135.0f <= FMath::Abs(MeshRotator.Roll) && FMath::Abs(MeshRotator.Roll) <= -45.0f;

	bool C3 = -215.0f <= FMath::Abs(MeshRotator.Roll) && FMath::Abs(MeshRotator.Roll) <= -145.0f;

	bool C4 = 215.0f >= FMath::Abs(MeshRotator.Roll) && FMath::Abs(MeshRotator.Roll) >= 145.0f;

	bool C5 = FMath::IsNearlyEqual(GetVehicleMovementComponent()->GetForwardSpeed(), 0.0f, 10.0f);

	if ((C0 || C1 || C2 || C3 || C4 ) && C5)
	{
		UE_LOG(LogTemp, Display, TEXT("UpsideDown"));
		AdjustRotation();
	}
}

void ADrone::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState"))
	UpdateMeshColor();
	UpdateName();
}

void ADrone::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("PossessedBy"))
	UpdateName();
	UpdateMeshColor();
	UpdateCollisionChannel();
}

void ADrone::RestoreDefaultBoost()
{
	ConsumeBoostRate = DefaultConsumeBoostRate;
}

void ADrone::RestoreDefaultFire()
{
	TimeBetweenShoots = DefaultTimeBetweenShots;

	if (GetWorldTimerManager().IsTimerActive(ContinuousFireTimer))
	{
		GetWorldTimerManager().SetTimer(ContinuousFireTimer, this, &ADrone::Fire, TimeBetweenShoots, true);
	}
}

void ADrone::ActivateRapidFire(float RapidTimeBetweenShots)
{
	TimeBetweenShoots = RapidTimeBetweenShots;

	if (GetWorldTimerManager().IsTimerActive(ContinuousFireTimer))
	{
		GetWorldTimerManager().SetTimer(ContinuousFireTimer, this, &ADrone::Fire, TimeBetweenShoots, true);
	}
}

void ADrone::ActivateInfiniteBoost()
{
	ConsumeBoostRate = 0.0f;
}

void ADrone::UpdateMeshColor()
{
	auto HSPlayerState = Cast<AHotStuffPlayerState>(PlayerState);

	if (HSPlayerState)
	{
		auto LocalHSPlayerState = Cast<AHotStuffPlayerState>(GetLocalPlayerState());
		if (LocalHSPlayerState)
		{
			SetupMeshColor(HSPlayerState->GetTeamNumber() != LocalHSPlayerState->GetTeamNumber());
		}
	}
}

void ADrone::UpdateName()
{
	if (PlayerState)
	{
		auto LocalPlayerState = GetLocalPlayerState();

		if (LocalPlayerState && PlayerState->GetPlayerName() != LocalPlayerState->GetPlayerName())
		{
			SetupInfoWidget(PlayerState->GetPlayerName());
		}
		else
		{
			SetupInfoWidget("");
		}
	}
}

void ADrone::UpdateCollisionChannel()
{
	UE_LOG(LogTemp, Log, TEXT("Updating collision channel"))

	auto HSPlayerState = Cast<AHotStuffPlayerState>(PlayerState);

	if (HSPlayerState)
	{
		SetupCollisionChannel(HSPlayerState->GetTeamNumber());
	}
}

float ADrone::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	LastDamageTimestamp = GetWorld()->GetRealTimeSeconds();
	if (!Dead && HealthPoints > 0.0f)
	{
		HealthPoints -= Damage;
		OnRep_HealthPoints();

		LastControllerThatFired = (APlayerController*)EventInstigator;

		if (!Dead && HealthPoints <= 0.0f)
		{
			ReleaseBomb();
			Dead = true;
			Death();
		}

		return Damage;
	}
	return 0.0f;
}

void ADrone::OnRep_HealthPoints()
{
	FadingDamage = PreviousHealth - HealthPoints;
	PreviousHealth = HealthPoints;
}

void ADrone::OnRep_LaunchForce()
{
	UpdateForceBar((int)LaunchForce);
}

void ADrone::DestroyDrone()
{
	if (HasAuthority())
	{
		auto HSGameMode = Cast<AHotStuffGameMode>(GetWorld()->GetAuthGameMode());
		if (!ensure(HSGameMode)) return;

		HSGameMode->HandleDroneKilled(Controller, LastControllerThatFired);

		Destroy(true, true);
	}
}

APlayerState * ADrone::GetLocalPlayerState()
{
	auto MyPlayerController = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
	if (!ensure(MyPlayerController)) return nullptr;

	return MyPlayerController->PlayerState;
}


