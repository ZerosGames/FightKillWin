// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Main_Base.generated.h"

UCLASS()
class MYPROJECT_API AMain_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMain_Base();

	UPROPERTY(Category = Scene, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class USceneComponent * Scene;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(Category = TargetSphere, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TargetSphere;

	TSubclassOf<class AActor> BP_Explosion;

	UFUNCTION(Server, Reliable, WithValidation = Server_Explode_Validate)
	void Server_Explode(AActor* _Explosion);
	virtual void Server_Explode_Implementation(AActor* _Explosion);
	virtual bool Server_Explode_Validate(AActor* _Explosion);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	UPROPERTY(Replicated)
	float Health;

};
