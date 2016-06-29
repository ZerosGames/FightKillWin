// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MainPawn.h"
#include "Engine.h"
#include "MainHUD.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////Pawn defaults///////////////////////////////////////

AMainPawn::AMainPawn()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> Mesh;
		FConstructorStatics()
			: Mesh(TEXT("/Game/Meshs/Steve_Asset/f14d"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetStaticMesh(ConstructorStatics.Mesh.Get());
	Mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
	Mesh->AttachTo(RootComponent);
	Mesh->SetIsReplicated(true);

	BoxCollidier = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollidier->SetRelativeLocation(FVector(-3, 0, 0));
	BoxCollidier->SetBoxExtent(FVector(5, 5, 1));
	BoxCollidier->OnComponentBeginOverlap.AddDynamic(this, &AMainPawn::OnBoxHit);
	BoxCollidier->OnComponentEndOverlap.AddDynamic(this, &AMainPawn::OnBoxOverlapEnd);
	BoxCollidier->AttachTo(Mesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 200.0f;
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 60.0f);
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 5.0f;
	SpringArm->CameraRotationLagSpeed = 10.0f;
	SpringArm->CameraLagMaxDistance = 150.0f;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	MovementComponent->MaxSpeed = 1000000.0f;

	SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SightSphere"));
	SightSphere->AttachTo(RootComponent);
	SightSphere->InitSphereRadius(10000.0f);
	SightSphere->OnComponentBeginOverlap.AddDynamic(this, &AMainPawn::OnOverlapBegin);
	SightSphere->OnComponentEndOverlap.AddDynamic(this, &AMainPawn::OnOverlapEnd);

	TargetSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetSphere"));
	TargetSphere->AttachTo(RootComponent);
	TargetSphere->InitSphereRadius(100.0f);
	TargetSphere->SetCollisionResponseToChannel(COLLISION_TARGET, ECR_Block);

	Crosshair = CreateDefaultSubobject<UArrowComponent>(TEXT("Crosshair"));
	Crosshair->AttachTo(RootComponent);
	Crosshair->SetRelativeLocation(FVector(4000.0f, 0, 0));

	GunSounds = CreateDefaultSubobject<UAudioComponent>(TEXT("Gun Sounds"));
	GunSounds->bAutoActivate = false;
	GunSounds->AttachTo(Mesh);

	static ConstructorHelpers::FClassFinder<AActor> BPExplosion(TEXT("/Game/Effects/ParticleActors/BP_PExplosion"));
	BP_Explosion = BPExplosion.Class;

	static ConstructorHelpers::FClassFinder<AActor> BPBaseweapon(TEXT("/Game/Blueprints/Weapons/BP_BaseWeapon"));
	BP_BaseWeapon = BPBaseweapon.Class;

	SetActorEnableCollision(true);

	MaxHealth = 100.0f;
	Acceleration = 0;
	MaxSpeed = 2000.0f;
	MinSpeed = 1000.0f;

	PitchTurnSpeed = 30.f;
	YawTurnSpeed = 30.f;
	RollTurnSpeed = 10.0f;

	MouseSensetivity = 1.0f;

	hasMatchStarted = false;
	Paused = false;
	Died = false;

	RollTimer = 10;
	bIsOnCooldown = false;
	bIsRolling = false;
	bRollStarted = false;
	RollingRotationStep = 10.0f;
	RollingPositionStep = 10.0f;
	OutOfBounds = false;
	CountOOBDownTimer = 20;
}

void AMainPawn::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	if (EngineParticle)
	{
		FVector Temp = FVector(0.1, 0.1, 0.1);
		EngineLeft = UGameplayStatics::SpawnEmitterAttached(EngineParticle, Mesh, "EngineL");
		EngineLeft->SetRelativeScale3D(Temp);
		EngineRight = UGameplayStatics::SpawnEmitterAttached(EngineParticle, Mesh, "EngineR");
		EngineRight->SetRelativeScale3D(Temp);
	}

	if (GetController())
	{
		if (GetController()->Tags.IsValidIndex(0))
		{
			this->Tags.Add(GetController()->Tags[0]);
		}
	}
}

void AMainPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (hasMatchStarted && !Paused)
	{
		ThrustInput();

		if (bIsRolling)
		{
			HandleExtendedMovement();
		}

		if (!bIsRolling)
		{
			RotateInput(Pitch, Yaw, MeshRoll, DeltaTime);

			if (CurrentWeapon != nullptr && Targeting == true)
			{
				CurrentWeapon->setTargeting(true);
			}
			else if(CurrentWeapon != nullptr)
			{
				CurrentWeapon->setTargeting(false);
			}

			if (IsFiring && CurrentWeapon != nullptr)
			{
				CurrentWeapon->OnStartFire();

				if (!GunSounds->IsPlaying())
				{
					GunSounds->Play(0.03f);
				}
			}
			else if (!IsFiring && CurrentWeapon != nullptr)
			{
				CurrentWeapon->OnStopFire();

				if (GunSounds->IsPlaying())
				{
					GunSounds->Stop();
				}
			}
		}
	}
	else if (!hasMatchStarted && Paused)
	{
		Speed = 0;
	}

	if (CurrentWeapon == nullptr)
	{
		if (Role == ROLE_Authority)
		{
			SpawnWeapon();
		}
	}

	if (Role == ROLE_Authority)
	{
		for (AMainPawn* temp : GetOverlapPawn())
		{
			AMainPlayerController* PC = Cast<AMainPlayerController>(temp->Controller);
			AMainPlayerController* MYPC = Cast<AMainPlayerController>(GetController());
			if (PC && MYPC)
			{
				if (PC->GetTeamNum() != MYPC->GetTeamNum())
				{
					if (!EnemysTempPawn.Contains(temp))
						EnemysTempPawn.Add(temp);
				}
				else
				{
					if (!FriendlysTempPawn.Contains(temp))
						FriendlysTempPawn.Add(temp);
				}
			}
		}

		for (AActor* temp : GetOverlapActor())
		{
			if (temp->ActorHasTag("BLUE"))
			{
				if (!EnemysTempActor.Contains(temp))
					EnemysTempActor.Add(temp);
			}
			else if (temp->ActorHasTag("RED"))
			{
				if (!FriendlysTempActor.Contains(temp))
					FriendlysTempActor.Add(temp);
			}
		}
	}
}

void AMainPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	//Input for firing//
	InputComponent->BindAction("Fire", IE_Pressed, this, &AMainPawn::FireInputBegin);
	InputComponent->BindAction("Fire", IE_Released, this, &AMainPawn::FireInputEnd);
	InputComponent->BindAction("ChangeWeaponUp", IE_Pressed, this, &AMainPawn::ChangeWeaponUp);
	InputComponent->BindAction("ChangeWeaponDown", IE_Pressed, this, &AMainPawn::ChangeWeaponDown);
	InputComponent->BindAction("StartTargeting", IE_Pressed, this, &AMainPawn::StartTargeting);
	InputComponent->BindAction("StopTargeting", IE_Released, this, &AMainPawn::StopTargeting);

	//Movement//
	InputComponent->BindAxis("Pitch", this, &AMainPawn::PitchInput);
	InputComponent->BindAxis("Yaw", this, &AMainPawn::YawInput);

	InputComponent->BindAction("Acelerate", IE_Pressed, this, &AMainPawn::AddAcceleration);
	InputComponent->BindAction("Acelerate", IE_Released, this, &AMainPawn::SubAcceleration);
	InputComponent->BindAction("Pause", IE_Pressed, this, &AMainPawn::Pause);
	InputComponent->BindAction("UnPause", IE_Pressed, this, &AMainPawn::UnPause);
	InputComponent->BindAction("FlipCamera", IE_Pressed, this, &AMainPawn::FlipCameraInputBegin);
	InputComponent->BindAction("FlipCamera", IE_Released, this, &AMainPawn::FlipCameraInputEnd);
	InputComponent->BindAction("SpaceStart", IE_Pressed, this, &AMainPawn::Start);

	//ExtendedMovement//
	InputComponent->BindAction("RollRight", IE_Pressed, this, &AMainPawn::ToggleRollRight);
	InputComponent->BindAction("RollLeft", IE_Pressed, this, &AMainPawn::ToggleRollLeft);

}

//////////////////////////////////////Movement////////////////////////////////////////////

void AMainPawn::ThrustInput()
{
	if (hasMatchStarted == true && !Paused)
	{
		if (Accelerate)
		{
			Acceleration = FMath::FInterpTo(Acceleration, 1.25f, GetWorld()->DeltaTimeSeconds, 2.0f);
		}
		else
		{
			Acceleration = FMath::FInterpTo(Acceleration, 1.0f, GetWorld()->DeltaTimeSeconds, 2.0f);
		}

		Speed = MinSpeed * Acceleration;

		if (Speed < MinSpeed)
		{
			Speed = MinSpeed;
		}
		else if (Speed > MaxSpeed)
		{
			Speed = MaxSpeed;
		}
		FVector FWV = GetActorForwardVector();
		ClientAddThrust(FWV, Speed);
	}
}

void AMainPawn::PitchInput(float Val)
{
	if (hasMatchStarted == true && !Paused)
	{

		float TargetPitchSpeed = (Val * PitchTurnSpeed * MouseSensetivity);

		CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

		AddControllerPitchInput(CurrentPitchSpeed * GetWorld()->GetDeltaSeconds());

		Pitch = CurrentPitchSpeed;
	}

}

void AMainPawn::YawInput(float Val)
{
	if (hasMatchStarted == true && !Paused)
	{
		const bool bIsTurning = FMath::Abs(Val) > 0.2f;


		float TargetYawSpeed = (Val * YawTurnSpeed * MouseSensetivity);
		float TargetRollSpeed = (Val * RollTurnSpeed * MouseSensetivity);


		TargetRollSpeed = FMath::Clamp(TargetRollSpeed, -90.0f, 90.0f);

		CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
		CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

		TargetRollSpeed = bIsTurning ? (CurrentRollSpeed * 0.5f) : (GetActorRotation().Roll * -1.75f);
		CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 1.0f);

		AddControllerYawInput(CurrentYawSpeed * GetWorld()->GetDeltaSeconds());
		AddControllerRollInput(CurrentRollSpeed * GetWorld()->GetDeltaSeconds());


		Yaw = CurrentYawSpeed;
		MeshRoll = CurrentRollSpeed;

		StrifeYaw = bIsTurning ? (Val * 10) : 0;

		CurrentStrifeYaw = FMath::FInterpTo(CurrentStrifeYaw, StrifeYaw, GetWorld()->GetDeltaSeconds(), 1.f);

		Camera->SetRelativeLocation(FVector(0, CurrentStrifeYaw, 0));

	}
}

void AMainPawn::RotateInput(float Pitch, float Yaw, float Roll, float DeltaTime)
{
	if (hasMatchStarted == true && !Paused)
	{
		float DeltaPitch = Pitch * DeltaTime;
		float DeltaYaw = Yaw * DeltaTime;
		float DeltaRoll = Roll * DeltaTime;

		FRotator rot(GetActorRotation().Pitch, GetActorRotation().Yaw, Roll);

		if ((GetActorRotation().Pitch > 80 && DeltaPitch > 0) || (GetActorRotation().Pitch < -80 && DeltaPitch < 0))
		{
			DeltaPitch = 0.0f;
		}

		ClientAddRotation(FRotator(DeltaPitch, DeltaYaw, PawnRoll), rot, Mesh);
		ServerAddRotation(GetActorRotation(), rot, Mesh);
	}
}

void AMainPawn::FlipCameraInputBegin()
{
	SpringArm->SetRelativeRotation(FQuat(0, 0, 180, 1));
}

void AMainPawn::FlipCameraInputEnd()
{
	SpringArm->SetRelativeRotation(FQuat(0, 0, 0, 1));
}

//////////////////////////////////////AdvancedMovement////////////////////////////////////

void AMainPawn::ToggleRollRight()
{
	if (!bIsOnCooldown)
	{
		RollingSign = 1.0f;
		TargetRotation = FRotator(0, 0, 360);

		bIsOnCooldown = true;
		bIsRolling = true;

		GetWorldTimerManager().SetTimer(RollTimerHandle, this, &AMainPawn::HandleRollCooldown, 1.0f, true);
	}
}

void AMainPawn::ToggleRollLeft()
{
	if (!bIsOnCooldown)
	{
		RollingSign = -1.0f;
		TargetRotation = FRotator(0, 0, -360);

		bIsOnCooldown = true;
		bIsRolling = true;

		GetWorldTimerManager().SetTimer(RollTimerHandle, this, &AMainPawn::HandleRollCooldown, 1.0f, true);
	}
}

void AMainPawn::HandleRollCooldown()
{
	RollTimer--;
	if (RollTimer < 1)
	{
		GetWorldTimerManager().ClearTimer(RollTimerHandle);
		RollTimer = 10;
		bIsOnCooldown = false;
	}
}

void AMainPawn::HandleExtendedMovement()
{
	if (!bRollStarted)
	{
		CurrentRotation = Mesh->GetComponentRotation();
		bRollStarted = true;
	}

	if (bRollStarted)
	{
		CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, TargetRotation.Roll, GetWorld()->DeltaTimeSeconds, RollingRotationStep);
		AddActorLocalOffset(FVector(0, RollingPositionStep * RollingSign, 0));
		Mesh->SetWorldRotation(CurrentRotation);

		if (CurrentRotation.Roll * RollingSign >= TargetRotation.Roll * RollingSign - 2.0f)
		{
			bIsRolling = false;
			bRollStarted = false;
		}
	}
}

//////////////////////////////////////Utils For Input////////////////////////////////////

void AMainPawn::StartTargeting()
{
	Targeting = true;
}

void AMainPawn::StopTargeting()
{
	Targeting = false;
}

void AMainPawn::FireInputBegin()
{
	IsFiring = true;
}

void AMainPawn::FireInputEnd()
{
	IsFiring = false;
}

void AMainPawn::Start()
{
	hasMatchStarted = true;
}

void AMainPawn::Pause()
{
	Paused = true;
}

void AMainPawn::UnPause()
{
	Paused = false;
}

void AMainPawn::AddAcceleration()
{
	Accelerate = true;
}

void AMainPawn::SubAcceleration()
{
	Accelerate = false;
}

//////////////////////////////////////Damage And Death/////////////////////////////////////

float AMainPawn::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;

		if (Health <= 0.0f)
		{
			//Die();
		}
		else
		{
			PlayHit(Damage, DamageEvent);
		}
	}
	return ActualDamage;
}

void AMainPawn::PlayHit(float _Damage, struct FDamageEvent const& _DamageEvent)
{
	AMainPlayerController* MyPC = Cast<AMainPlayerController>(Controller);
	AMainHUD* MyHUD = MyPC ? Cast<AMainHUD>(MyPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, "Got here");
		MyHUD->NotifyWeaponHit(_Damage, _DamageEvent, this);
	}
}

void AMainPawn::Die()
{
	AMainPlayerController* MyPlayerController = Cast<AMainPlayerController>(Controller);
	if (MyPlayerController)
	{
		DetachFromControllerPendingDestroy();
		CurrentWeapon->Destroy();
		MyPlayerController->HandleRespawn(GetActorLocation());
		ServerExplosion(BP_Explosion.GetDefaultObject());
		Destroy();
	}
}

//////////////////////////////////////Collision////////////////////////////////////////////

void AMainPawn::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp && OtherComp->GetName() != "SightSphere" && OtherComp->GetName() != "TargetSphere" && !OtherActor->IsA(AMainPawn::StaticClass()))
	{
		tempActor.Add(OtherActor);
	}
	else if (OtherActor && (OtherActor != this) && OtherComp && OtherComp->GetName() != "SightSphere" && OtherComp->GetName() != "TargetSphere" && OtherActor->IsA(AMainPawn::StaticClass()))
	{
		AMainPawn* Pawn = Cast<AMainPawn>(OtherActor);
		tempPawn.Add(Pawn);
	}
}

void AMainPawn::OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp && OtherComp->GetName() != "SightSphere" && OtherComp->GetName() != "TargetSphere")
	{
		tempActor.Remove(OtherActor);
	}
}

void AMainPawn::OnBoxHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, OtherActor->GetName());

	if (!OtherActor->GetName().Contains("Controller") && !OtherActor->GetName().Contains("Bounds"))
	{
		//Die();
	}
	else if (OtherActor->GetName().Contains("Bounds") && OutOfBounds)
	{
		OutOfBounds = false;
		CountOOBDownTimer = 20;
		GetWorldTimerManager().ClearTimer(CountOOBDownTimerHandle);
		AMainPlayerController* PC = Cast<AMainPlayerController>(GetController());
		if (PC)
		{
			PC->OutOfBounds();
		}
	}
}

void AMainPawn::OnBoxOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetName().Contains("Bound"))
	{
		OutOfBounds = true;
		GetWorldTimerManager().SetTimer(CountOOBDownTimerHandle, this, &AMainPawn::OutOfBoundsTimer, 1.0f, true);
		AMainPlayerController* PC = Cast<AMainPlayerController>(GetController());
		if (PC)
		{
			PC->OutOfBounds();
		}
	}
}

////////////////////////////////Timers/////////////////////////////////////////////////////

void AMainPawn::OutOfBoundsTimer()
{
	CountOOBDownTimer--;

	if (CountOOBDownTimer < 1)
	{
		AMainPlayerController* PC = Cast<AMainPlayerController>(GetController());
		if (PC)
		{
			PC->OutOfBounds();
		}

		Die();
		GetWorldTimerManager().ClearTimer(CountOOBDownTimerHandle);
		CountOOBDownTimer = 20;
	}
}

////////////////////////////////Weapon's and Firing////////////////////////////////////////

void AMainPawn::StartFiring()
{
	CurrentWeapon->OnStartFire();
}

void AMainPawn::ChangeWeaponUp()
{
	if (!Died)
	{
		WeaponID += 1;

		if (WeaponID > 2)
		{
			WeaponID = 0;
		}

		if (CurrentWeapon != nullptr)
		{
			CurrentWeapon->ChangeWeapon(WeaponID);
		}
	}
}

void AMainPawn::ChangeWeaponDown()
{
	if (!Died)
	{
		WeaponID -= 1;

		if (WeaponID < 0)
		{
			WeaponID = 2;
		}

		if (CurrentWeapon != nullptr)
		{
			CurrentWeapon->ChangeWeapon(WeaponID);
		}
	}
}

void AMainPawn::SpawnWeapon()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	CurrentWeapon = Cast<ABaseWeapon>(GetWorld()->SpawnActor(BP_BaseWeapon));
	EquipWeapon(CurrentWeapon);
}

void AMainPawn::SetCurrentWeapon(ABaseWeapon* BaseWeapon)
{
	CurrentWeapon = BaseWeapon;
	CurrentWeapon->ChangeWeapon(0);
	CurrentWeapon->SetOwningPawn(this);
}

void AMainPawn::OnRep_SpawnedWeapon(ABaseWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon);
}

void AMainPawn::TargetedByMissle(AActor* _Target)
{
	if (_Target != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Black, "You Have Been Targeted By: " + _Target->GetName());
		TargetedActor = _Target;
	}
}

/////////////////////////////////////Client Functions/////////////////////////////////

void AMainPawn::ClientAddThrust_Implementation(FVector Transform, float Thrust)
{
	AddMovementInput(Transform, Thrust / MaxSpeed);
	ServerAddThrust(GetActorLocation());
}

void AMainPawn::ClientAddRotation_Implementation(FRotator Rotation, FRotator MeshRotation, UStaticMeshComponent* Mesh)
{
	AddActorLocalRotation(Rotation);
	SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, 0));
	Mesh->SetWorldRotation(MeshRotation);
}

/////////////////////////////////////Server Functions/////////////////////////////////

bool AMainPawn::ServerAddThrust_Validate(FVector Transform)
{
	return true;
}

void AMainPawn::ServerAddThrust_Implementation(FVector Transform)
{
	SetActorLocation(Transform);
}

bool AMainPawn::ServerAddRotation_Validate(FRotator Rotation, FRotator MeshRotation, UStaticMeshComponent* Mesh)
{
	return true;
}

void AMainPawn::ServerAddRotation_Implementation(FRotator Rotation, FRotator MeshRotation, UStaticMeshComponent* Mesh)
{
	Mesh->SetWorldRotation(MeshRotation);
	SetActorRotation(Rotation);
}

void AMainPawn::EquipWeapon(ABaseWeapon* Weapon)
{
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

bool AMainPawn::ServerEquipWeapon_Validate(ABaseWeapon* Weapon)
{
	return true;
}

void AMainPawn::ServerEquipWeapon_Implementation(ABaseWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

void AMainPawn::ServerExplosion_Implementation(AActor* _Explosion)
{
	FActorSpawnParameters Params;
	AActor* Explosion = GetWorld()->SpawnActor<AActor>(_Explosion->GetClass(), GetActorLocation(), GetActorRotation(), Params);
}

bool AMainPawn::ServerExplosion_Validate(AActor* _Explosion)
{
	return true;
}

/////////////////////////////////////Getters//////////////////////////////////////////

ABaseWeapon* AMainPawn::getWeapon()
{
	return CurrentWeapon;
}

AActor* AMainPawn::GetTarget()
{
	if (CurrentWeapon != nullptr)
	{
		return CurrentWeapon->getTarget();
	}
	return nullptr;
}

TArray<AActor*> AMainPawn::GetOverlapActor()
{
	return tempActor;
}

TArray<AMainPawn*> AMainPawn::GetOverlapPawn()
{
	return tempPawn;
}

TArray<AActor*> AMainPawn::GetFriendlysOverlapActor()
{
	return FriendlysTempActor;
}

TArray<AActor*> AMainPawn::GetEnemyOverlapActor()
{
	return EnemysTempActor;
}

TArray<AMainPawn*> AMainPawn::GetFriendlysOverlapPawn()
{
	return FriendlysTempPawn;
}

TArray<AMainPawn*> AMainPawn::GetEnemyOverlapPawn()
{
	return EnemysTempPawn;
}

float AMainPawn::GetSpeed()
{
	return Speed;
}

AController* AMainPawn::GetPC()
{
	return MyPc;
}

////////////////////////////////////Replication////////////////////////////////////////

void AMainPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainPawn, CurrentWeapon);
	DOREPLIFETIME(AMainPawn, Health);
	DOREPLIFETIME(AMainPawn, MyPc);
	DOREPLIFETIME(AMainPawn, FriendlysTempPawn);
	DOREPLIFETIME(AMainPawn, EnemysTempPawn);
	DOREPLIFETIME(AMainPawn, FriendlysTempActor);
	DOREPLIFETIME(AMainPawn, EnemysTempActor);
	DOREPLIFETIME(AMainPawn, hasMatchStarted);
	DOREPLIFETIME(AMainPawn, GunSounds);
}


