// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BaseWeapon.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	HitActor = nullptr;
	StartFiring = false;
	RightFired = false;
	LastFireTime = 0.0f;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	ProjectileType = EWeaponProjectile::EBullet;
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyPawn != nullptr)
	{
		fireTargetingRaycasts();

		if (ProjectileType != EWeaponProjectile::EProjectile)
		{
			setInstantTarget();
		}
		else
		{
			setProjectileTarget();
		}
	}

	if (StartFiring)
	{
		HandleFiring();
	}

	if (ProjectileType == NULL)
	{
		ProjectileType = EWeaponProjectile::EBullet;
	}
}

void ABaseWeapon::OnStartFire()
{
	if (!StartFiring)
	{
		StartFiring = true;
		HandleFiring();
	}
}

void ABaseWeapon::OnStopFire()
{
	if (StartFiring)
	{
		GetWorldTimerManager().ClearTimer(time_handler);
		StopSpawnParticle();
		StartFiring = false;
	}
}

void ABaseWeapon::HandleFiring()
{
	const float GameTime = GetWorld()->GetTimeSeconds();

	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0 && LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(time_handler, this, &ABaseWeapon::Fire, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		Fire();
	}
}

void ABaseWeapon::Fire()
{
	switch (ProjectileType)
	{
	case EWeaponProjectile::EBullet:
		Instant_Fire();
		LastFireTime = GetWorld()->GetTimeSeconds();
		GetWorldTimerManager().SetTimer(time_handler, this, &ABaseWeapon::Fire, WeaponConfig.TimeBetweenShots, false);
		break;
	case EWeaponProjectile::ESniper:
		Instant_Fire();
		LastFireTime = GetWorld()->GetTimeSeconds();
		GetWorldTimerManager().SetTimer(time_handler, this, &ABaseWeapon::Fire, WeaponConfig.TimeBetweenShots, false);
		break;
	case EWeaponProjectile::EProjectile:
		if (MissleWeaponConfig.CurrentAmmo > 0 && Target)
		{
			Server_ProjectileFire(WeaponConfig, Target);
			MissleWeaponConfig.CurrentAmmo--;
			LastFireTime = GetWorld()->GetTimeSeconds();
			GetWorldTimerManager().SetTimer(time_handler, this, &ABaseWeapon::Fire, WeaponConfig.TimeBetweenShots, false);
		}
		else
		{
			LastFireTime = GetWorld()->GetTimeSeconds();
			GetWorldTimerManager().ClearTimer(time_handler);
		}
		break;
	}
}

void ABaseWeapon::SpawnParticle(FVector EndPoint)
{
	if (SniperMuzzleFX && MachineGunMuzzleFX && MachineGunTracer)
	{
		if (ProjectileType == EWeaponProjectile::ESniper)
		{
			if (FlashParticleL == nullptr && FlashParticleR == nullptr && TracerParticleR == nullptr && TracerParticleL == nullptr)
			{
				MyPawn->GetMesh()->GetSocketLocation(MuzzleAttachPointL);
				FlashParticleL = UGameplayStatics::SpawnEmitterAttached(SniperMuzzleFX, MyPawn->GetMesh(), MuzzleAttachPointL);
				//TracerParticleL = UGameplayStatics::SpawnEmitterAttached(MachineGunTracer, MyPawn->GetMesh(), MuzzleAttachPointL);
				//TracerParticleL->SetVectorParameter("ShockBeamEnd", EndPoint);
				FlashParticleL->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				//TracerParticleL->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				FlashParticleL->bOwnerNoSee = false;
				//TracerParticleL->bOnlyOwnerSee = false;

				MyPawn->GetMesh()->GetSocketLocation(MuzzleAttachPointR);
				FlashParticleR = UGameplayStatics::SpawnEmitterAttached(SniperMuzzleFX, MyPawn->GetMesh(), MuzzleAttachPointR);
				//TracerParticleR = UGameplayStatics::SpawnEmitterAttached(MachineGunTracer, MyPawn->GetMesh(), MuzzleAttachPointR);
				//TracerParticleR->SetVectorParameter("ShockBeamEnd", EndPoint);
				FlashParticleR->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				//TracerParticleR->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				FlashParticleR->bOwnerNoSee = false;
				//TracerParticleR->bOwnerNoSee = false;
			}
		}
		else
		{
			if (FlashParticleL == nullptr && FlashParticleR == nullptr && TracerParticleR == nullptr && TracerParticleL == nullptr)
			{
				MyPawn->GetMesh()->GetSocketLocation(MuzzleAttachPointL);
				FlashParticleL = UGameplayStatics::SpawnEmitterAttached(MachineGunMuzzleFX, MyPawn->GetMesh(), MuzzleAttachPointL);
				//TracerParticleL = UGameplayStatics::SpawnEmitterAttached(MachineGunTracer, MyPawn->GetMesh(), MuzzleAttachPointL);
				//TracerParticleL->SetVectorParameter("ShockBeamEnd", EndPoint);
				FlashParticleL->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				//TracerParticleL->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				FlashParticleL->bOwnerNoSee = false;
				//TracerParticleL->bOnlyOwnerSee = false;

				MyPawn->GetMesh()->GetSocketLocation(MuzzleAttachPointR);
				FlashParticleR = UGameplayStatics::SpawnEmitterAttached(MachineGunMuzzleFX, MyPawn->GetMesh(), MuzzleAttachPointR);
				//TracerParticleR = UGameplayStatics::SpawnEmitterAttached(MachineGunTracer, MyPawn->GetMesh(), MuzzleAttachPointR);
				//TracerParticleR->SetVectorParameter("ShockBeamEnd", EndPoint);
				FlashParticleR->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				//TracerParticleR->RelativeScale3D = FVector(0.1, 0.1, 0.1);
				FlashParticleR->bOwnerNoSee = false;
				//TracerParticleR->bOwnerNoSee = false;
			}
		}

		if (TracerParticleL && TracerParticleR)
		{
			//TracerParticleR->SetVectorParameter("ShockBeamEnd", EndPoint);
			//TracerParticleL->SetVectorParameter("ShockBeamEnd", EndPoint);
		}
	}
}

void ABaseWeapon::StopSpawnParticle()
{
	if (FlashParticleL && FlashParticleR)
	{
		FlashParticleL->DeactivateSystem();
		FlashParticleL = nullptr;

		FlashParticleR->DeactivateSystem();
		FlashParticleR = nullptr;
	}

	if (TracerParticleL && TracerParticleR)
	{
		TracerParticleL->DeactivateSystem();
		TracerParticleL = nullptr;

		TracerParticleR->DeactivateSystem();
		TracerParticleR = nullptr;
	}
}

void ABaseWeapon::setTargeting(bool _Targeting)
{
	targeting = _Targeting;
}

void ABaseWeapon::ChangeWeapon(int WeaponID)
{
	switch (WeaponID)
	{
	case 0:
		ProjectileType = EWeaponProjectile::EBullet;
		WeaponConfig = MachineGunWeaponConfig;
		break;
	case 1:
		ProjectileType = EWeaponProjectile::ESniper;
		WeaponConfig = SniperWeaponConfig;
		break;
	case 2:
		ProjectileType = EWeaponProjectile::EProjectile;
		WeaponConfig = MissleWeaponConfig;
		break;
	}
}

void ABaseWeapon::ReloadAmmo()
{
	if (CurrentAmmo > 0)
	{
		if (CurrentAmmo < WeaponConfig.MaxClip)
		{
			CurrentClip = CurrentAmmo;
		}
		else
		{
			CurrentAmmo -= WeaponConfig.MaxClip;
			CurrentClip += WeaponConfig.MaxClip;
		}
	}
	else
	{
	}
}

void ABaseWeapon::Instant_Fire()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = WeaponConfig.WeaponSpread;
	const float SpreadCone = FMath::DegreesToRadians(WeaponConfig.WeaponSpread * 0.5);
	const FVector AimDir = MyPawn->GetActorForwardVector();
	const FVector StartTrace = MyPawn->GetActorLocation();
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, SpreadCone, SpreadCone);
	FVector EndTrace;

	if (Target)
	{
		FVector TargetDir = (MyPawn->GetActorLocation() - Target->GetActorLocation());
		TargetDir.Normalize();
		FVector ShootDir2 = WeaponRandomStream.VRandCone(-TargetDir, SpreadCone, SpreadCone);
		EndTrace = StartTrace + ShootDir2 * WeaponConfig.WeaponRange;
	}
	else
	{
		EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;
	}

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	SpawnParticle(EndTrace);
	HitActor = Cast<AActor>(Impact.GetActor());

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 1.0f);

	if (HitActor)
	{
		Server_DealDamage(Impact, ShootDir, WeaponConfig);
	}
}

FHitResult ABaseWeapon::WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, MyPawn->Controller);
	TraceParams.TraceTag = WeaponFireTag;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(MyPawn);

	FHitResult Hit(ForceInit);

	MyPawn->GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	return Hit;
}

void ABaseWeapon::Server_ProjectileFire_Implementation(FWeaponData data, AActor* _Target)
{
	if (MissleProjectile != nullptr && MyPawn != nullptr)
	{
		FVector R_MFlocation = MyPawn->GetMesh()->GetSocketLocation("Gun_Right");
		FRotator R_MFRotation = MyPawn->GetMesh()->GetSocketRotation("Gun_Right");
		FVector L_MFlocation = MyPawn->GetMesh()->GetSocketLocation("Gun_Left");
		FRotator L_MFRotation = MyPawn->GetMesh()->GetSocketRotation("Gun_Left");

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		if (RightFired == true)
		{
			AMissleProjectile* LeftMissle = GetWorld()->SpawnActor<AMissleProjectile>(MissleProjectile, L_MFlocation, L_MFRotation, SpawnParams);
			if (LeftMissle)
			{
				LeftMissle->setOwner(MyPawn);
				LeftMissle->setDmgAmount(data.Damage);
				FVector const LaunchDirL = L_MFRotation.Vector();
				LeftMissle->InitalVelocity(LaunchDirL);
				if (_Target != nullptr)
				{
					LeftMissle->setTargetactor(_Target);
				}
			}
			RightFired = false;
		}
		else if (RightFired == false)
		{
			AMissleProjectile* RightMissle = GetWorld()->SpawnActor<AMissleProjectile>(MissleProjectile, R_MFlocation, R_MFRotation, SpawnParams);
			if (RightMissle)
			{
				RightMissle->setOwner(MyPawn);
				RightMissle->setDmgAmount(data.Damage);
				FVector const LaunchDirR = R_MFRotation.Vector();
				RightMissle->InitalVelocity(LaunchDirR);
				if (_Target != nullptr)
				{
					RightMissle->setTargetactor(_Target);
				}
			}
			RightFired = true;
		}
	}
}

bool ABaseWeapon::Server_ProjectileFire_Validate(FWeaponData data, AActor* Target)
{
	return true;
}

void ABaseWeapon::fireTargetingRaycasts()
{

	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = 2.0f;
	const float SpreadCone = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	FVector Up_StartTrace = (MyPawn->GetActorLocation() + (MyPawn->GetActorForwardVector() * 500)) + (MyPawn->GetActorUpVector() * 30);
	FVector Up_Dir = MyPawn->GetActorForwardVector();
	FVector Up_EndTrace = Up_StartTrace + Up_Dir * (WeaponConfig.WeaponRange);

	FVector Down_StartTrace = (MyPawn->GetActorLocation() + (MyPawn->GetActorForwardVector() * 500)) + (MyPawn->GetActorUpVector() * -30);
	FVector Down_Dir = MyPawn->GetActorForwardVector();
	FVector Down_EndTrace = Down_StartTrace + Down_Dir * (WeaponConfig.WeaponRange);

	FVector Left_StartTrace = (MyPawn->GetActorLocation() + (MyPawn->GetActorForwardVector() * 500)) + (MyPawn->GetActorRightVector() * -30);
	FVector Left_Dir = MyPawn->GetActorForwardVector();
	FVector Left_EndTrace = Left_StartTrace + Left_Dir * (WeaponConfig.WeaponRange);

	FVector Right_StartTrace = (MyPawn->GetActorLocation() + (MyPawn->GetActorForwardVector() * 500)) + (MyPawn->GetActorRightVector() * 30);
	FVector Right_Dir = MyPawn->GetActorForwardVector();
	FVector Right_EndTrace = Right_StartTrace + Right_Dir * (WeaponConfig.WeaponRange);

	FName TraceTag("TraceTag");

	FCollisionQueryParams TraceParams(TraceTag, true, this);

	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(MyPawn);
	TraceParams.TraceTag = TraceTag;

	GetWorld()->LineTraceSingleByChannel(Up_hit, Up_StartTrace, Up_EndTrace, COLLISION_TARGET, TraceParams);
	//DrawDebugLine(GetWorld(), Up_StartTrace, Up_EndTrace, FColor::Cyan, false, -1.0f, 0, 2);

	GetWorld()->LineTraceSingleByChannel(Down_hit, Down_StartTrace, Down_EndTrace, COLLISION_TARGET, TraceParams);
	//DrawDebugLine(GetWorld(), Down_StartTrace, Down_EndTrace, FColor::Cyan, false, -1.0f, 0, 2);

	GetWorld()->LineTraceSingleByChannel(Left_hit, Left_StartTrace, Left_EndTrace, COLLISION_TARGET, TraceParams);
	//DrawDebugLine(GetWorld(), Left_StartTrace, Left_EndTrace, FColor::Cyan, false, -1.0f, 0, 2);

	GetWorld()->LineTraceSingleByChannel(Right_hit, Right_StartTrace, Right_EndTrace, COLLISION_TARGET, TraceParams);
	//DrawDebugLine(GetWorld(), Right_StartTrace, Right_EndTrace, FColor::Cyan, false, -1.0f, 0, 2);

}

void ABaseWeapon::setProjectileTarget()
{
	if (targeting)
	{
		if (MyPawn)
		{
			FVector MyPawnVector = MyPawn->GetActorForwardVector();
			MyPawnVector.Normalize();

			if (Up_hit.GetActor() && Target == nullptr)
			{
				FVector UpHitActor = Up_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				UpHitActor.Normalize();

				float Updot = FVector::DotProduct(MyPawnVector, UpHitActor);
				Updot = FMath::Acos(Updot) * 180.0f / PI;

				if (Updot < 30)
				{
					Target = Up_hit.GetActor();
				}
			}

			if (Down_hit.GetActor() && Target == nullptr)
			{
				FVector DownHitActor = Down_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				DownHitActor.Normalize();

				float Downdot = FVector::DotProduct(MyPawnVector, DownHitActor);
				Downdot = FMath::Acos(Downdot) * 180.0f / PI;

				if (Downdot < 30)
				{
					Target = Down_hit.GetActor();
				}
			}

			if (Left_hit.GetActor() && Target == nullptr)
			{
				FVector LeftHitActor = Left_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				LeftHitActor.Normalize();

				float Leftdot = FVector::DotProduct(MyPawnVector, LeftHitActor);
				Leftdot = FMath::Acos(Leftdot) * 180.0f / PI;

				if (Leftdot < 30)
				{
					Target = Left_hit.GetActor();
				}
			}

			if (Right_hit.GetActor() && Target == nullptr)
			{
				FVector RightHitActor = Right_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				RightHitActor.Normalize();

				float Rightdot = FVector::DotProduct(MyPawnVector, RightHitActor);
				Rightdot = FMath::Acos(Rightdot) * 180.0f / PI;

				if (Rightdot < 30)
				{
					Target = Right_hit.GetActor();
				}
			}
		}
	}

	if (Up_hit.GetActor() == nullptr && Down_hit.GetActor() == nullptr && Left_hit.GetActor() == nullptr && Right_hit.GetActor() == nullptr || targeting == false)
	{
		Target = nullptr;
	}
}

void ABaseWeapon::setInstantTarget()
{
	if (targeting)
	{
		if (MyPawn)
		{
			FVector MyPawnVector = MyPawn->GetActorForwardVector();
			MyPawnVector.Normalize();

			if (Up_hit.GetActor() && Target == nullptr)
			{
				FVector UpHitActor = Up_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				UpHitActor.Normalize();

				float Updot = FVector::DotProduct(MyPawnVector, UpHitActor);
				Updot = FMath::Acos(Updot) * 180.0f / PI;

				if (Updot < 30)
				{
					Target = Up_hit.GetActor();
				}
			}

			if (Down_hit.GetActor() && Target == nullptr)
			{
				FVector DownHitActor = Down_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				DownHitActor.Normalize();

				float Downdot = FVector::DotProduct(MyPawnVector, DownHitActor);
				Downdot = FMath::Acos(Downdot) * 180.0f / PI;

				if (Downdot < 30)
				{
					Target = Down_hit.GetActor();
				}
			}

			if (Left_hit.GetActor() && Target == nullptr)
			{
				FVector LeftHitActor = Left_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				LeftHitActor.Normalize();

				float Leftdot = FVector::DotProduct(MyPawnVector, LeftHitActor);
				Leftdot = FMath::Acos(Leftdot) * 180.0f / PI;

				if (Leftdot < 30)
				{
					Target = Left_hit.GetActor();
				}
			}

			if (Right_hit.GetActor() && Target == nullptr)
			{
				FVector RightHitActor = Right_hit.GetActor()->GetActorLocation() - MyPawn->GetActorLocation();

				RightHitActor.Normalize();

				float Rightdot = FVector::DotProduct(MyPawnVector, RightHitActor);
				Rightdot = FMath::Acos(Rightdot) * 180.0f / PI;

				if (Rightdot < 30)
				{
					Target = Right_hit.GetActor();
				}
			}
		}
	}

	if (Up_hit.GetActor() == nullptr && Down_hit.GetActor() == nullptr && Left_hit.GetActor() == nullptr && Right_hit.GetActor() == nullptr || targeting == false)
	{
		Target = nullptr;
	}
}

void ABaseWeapon::Server_DealDamage_Implementation(const FHitResult &Impact, const FVector &shootDir, FWeaponData Weaponconfig)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = UDamageType::StaticClass();
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = shootDir;
	PointDmg.Damage = Weaponconfig.Damage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}

bool ABaseWeapon::Server_DealDamage_Validate(const FHitResult &Impact, const FVector &shootDir, FWeaponData Weaponconfig)
{
	return true;
}

void ABaseWeapon::SetOwningPawn(AMainPawn* Pawn)
{
	if (Role = ROLE_Authority)
	{
		if (MyPawn != Pawn)
		{
			Instigator = Pawn;
			MyPawn = Pawn;
			SetOwner(Pawn->Controller);
		}
	}

}

void ABaseWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		SetOwningPawn(MyPawn);
	}
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, MyPawn);
}

AActor* ABaseWeapon::getTarget()
{
	return Target;
}
