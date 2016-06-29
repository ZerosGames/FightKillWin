// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MissleProjectile.h"


// Sets default values
AMissleProjectile::AMissleProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicateMovement = true;
	bReplicates = true;

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> Mesh;
		FConstructorStatics()
			: Mesh(TEXT("/Game/Meshs/Steve_Asset/Missle"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	MissleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MissleMesh->SetStaticMesh(ConstructorStatics.Mesh.Get());
	MissleMesh->SetRelativeScale3D(FVector(2, 2, 2));
	RootComponent = MissleMesh;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collider"));
	//CapsuleCollider->OnComponentBeginOverlap.AddDynamic(this, &AMissleProjectile::OnHit);
	CapsuleCollider->SetRelativeRotation(FRotator(90, 0, 0));
	CapsuleCollider->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	CapsuleCollider->AttachTo(RootComponent);

	MissleMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	MissleMovement->UpdatedComponent = MissleMesh;
	MissleMovement->InitialSpeed = 1000.0f;
	MissleMovement->MaxSpeed = 1000.0f;
	MissleMovement->bRotationFollowsVelocity = true;
	MissleMovement->bShouldBounce = false;
	MissleMovement->ProjectileGravityScale = 0.0f;
	MissleMovement->Velocity = FVector(0, 0, 0);

	static ConstructorHelpers::FClassFinder<AActor> BPExplosion(TEXT("/Game/Effects/ParticleActors/BP_PExplosionM"));
	BP_Explosion = BPExplosion.Class;
}

// Called when the game starts or when spawned
void AMissleProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMissleProjectile::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

		if (TargetActor != nullptr)
		{
			NotifyTarget(this);

			FVector WantedDir = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			WantedDir += TargetActor->GetVelocity() * WantedDir.Size() / MissleMovement->MaxSpeed;
			
			MissleMovement->Velocity += WantedDir * MissleMovement->InitialSpeed * 100.0f * DeltaTime;
			MissleMovement->Velocity = MissleMovement->Velocity.GetSafeNormal() * MissleMovement->MaxSpeed;

			FVector Dis = GetActorLocation() - (TargetActor->GetActorLocation());
			if (Dis.Size() < 5)
			{
				Server_Explode(TargetActor, BP_Explosion.GetDefaultObject(), Damage);
			}
		}
		else
		{
			NotifyTarget(nullptr);
			SetLifeSpan(1.0f);
		}
}

void AMissleProjectile::NotifyTarget(AActor* _temp)
{
	AMainPawn* Target = Cast<AMainPawn>(TargetActor);
	if (Target != nullptr)
	{
		Target->TargetedByMissle(_temp);
	}
}

void AMissleProjectile::NotifyTargetChange()
{
	TargetActor = nullptr;
}

void AMissleProjectile::Server_Explode_Implementation(AActor* _TargetActor, AActor* _Explosion, float _Damage)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = UDamageType::StaticClass();
	PointDmg.Damage = _Damage;

	FActorSpawnParameters Params;
	AActor* Explosion = GetWorld()->SpawnActor<AActor>(_Explosion->GetClass(), GetActorLocation(), GetActorRotation(), Params);

	_TargetActor->TakeDamage(PointDmg.Damage, PointDmg, _TargetActor->GetInstigatorController(), this);
	
	Destroy();
}

bool AMissleProjectile::Server_Explode_Validate(AActor* _TargetActor, AActor* _Explosion, float _Damage)
{
	return true;
}

void AMissleProjectile::OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AMissleProjectile::setTargetactor(AActor* _Target)
{
	TargetActor = _Target;
}

void AMissleProjectile::InitalVelocity(FVector velocity)
{
	MissleMovement->Velocity = velocity * MissleMovement->InitialSpeed;
}

void AMissleProjectile::setDmgAmount(float _dmg)
{
	Damage = _dmg;
}

void AMissleProjectile::setOwner(AMainPawn* _Owner)
{
	Owner = _Owner;
	SetOwner(_Owner);
	CapsuleCollider->IgnoreActorWhenMoving(Owner,true);
	MissleMesh->IgnoreActorWhenMoving(Owner, true);
}

