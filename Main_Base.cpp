// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "Main_Base.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AMain_Base::AMain_Base()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachTo(RootComponent);

	TargetSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetSphere"));
	TargetSphere->AttachTo(RootComponent);
	TargetSphere->InitSphereRadius(100.0f);
	TargetSphere->SetCollisionResponseToChannel(COLLISION_TARGET, ECR_Block);
	TargetSphere->AttachTo(Mesh);

	static ConstructorHelpers::FClassFinder<AActor> BPExplosion(TEXT("/Game/Effects/ParticleActors/BP_PExplosion"));
	BP_Explosion = BPExplosion.Class;

	Health = 100;

}

// Called when the game starts or when spawned
void AMain_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMain_Base::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

float AMain_Base::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;

		if (Health <= 0.0f)
		{
			Health = 0;
			AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				GameMode->NotifyGameEnd(this);
			}
			
		}
	}
	return ActualDamage;
}

void AMain_Base::Server_Explode_Implementation(AActor* _Explosion)
{
	
	FActorSpawnParameters Params;
	AActor* Explosion = GetWorld()->SpawnActor<AActor>(_Explosion->GetClass(), GetActorLocation(), GetActorRotation(), Params);
	
	Destroy();
}

bool AMain_Base::Server_Explode_Validate( AActor* _Explosion)
{
	return true;
}

void AMain_Base::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMain_Base, Health);
}

