// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MissleProjectile.generated.h"

UCLASS()
class MYPROJECT_API AMissleProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissleProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleDefaultsOnly, Category = ProjectileMesh)
	UStaticMeshComponent* MissleMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Collider)
	UCapsuleComponent* CapsuleCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	UProjectileMovementComponent* MissleMovement;

	UFUNCTION()
		void OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void InitalVelocity(FVector);
	void setTargetactor(AActor* _Target);
	void setDmgAmount(float _dmg);
	void setOwner(AMainPawn* Owner);

	void NotifyTarget(AActor* _Temp);
	void NotifyTargetChange();

	UFUNCTION(Server, Reliable, WithValidation = Server_Explode_Validate)
		void Server_Explode(AActor* _TargetActor, AActor* _Explosion, float _Damage);
	virtual void Server_Explode_Implementation(AActor* _TargetActor, AActor* _Explosion, float _Damage);
	virtual bool Server_Explode_Validate(AActor* _TargetActor, AActor* _Explosion, float _Damage);

	float Damage;
	TSubclassOf<class AActor> BP_Explosion;
	AMainPawn* Owner;
	AActor* TargetActor;
};
