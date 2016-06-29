// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainPawn.h"
#include "MissleProjectile.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

namespace EWeaponProjectile
{
	enum ProjectileType
	{
		EBullet			UMETA(DisplayName = "Bullet"),
		ESniper			UMETA(DisplayName = "Sniper"),
		EProjectile		UMETA(DisplayName = "Projectile"),
	};
}

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	int32 Damage;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	int32 CurrentAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	int32 MaxAmmo;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	int32 MaxClip;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	int32 ShotCost;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	int32 Priority;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	float TimeBetweenShots;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	float WeaponRange;
	UPROPERTY(EditDefaultsOnly, Category = "WeaponData")
	float WeaponSpread;
};

UCLASS()
class MYPROJECT_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void OnStartFire();
	void Fire();
	void SpawnParticle(FVector EndPoint);
	void StopSpawnParticle();

	void OnStopFire();
	void HandleFiring();

	void Instant_Fire();

	void setTargeting(bool);
	
	UFUNCTION(Server, Reliable, WithValidation = Server_ProjectileFire_Validate)
		void Server_ProjectileFire(FWeaponData data, AActor* Target);
	virtual void Server_ProjectileFire_Implementation(FWeaponData data, AActor* Target);
	virtual bool Server_ProjectileFire_Validate(FWeaponData data, AActor* Target);
	//void ProjectileFire();

	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = SniperConfig)
	FWeaponData SniperWeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = MachineGunConfig)
	FWeaponData MachineGunWeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = MissleConfig)
	FWeaponData MissleWeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = MissleProjectile)
	TSubclassOf<class AMissleProjectile> MissleProjectile;

	TEnumAsByte<EWeaponProjectile::ProjectileType> ProjectileType;
	
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName MuzzleAttachPointL;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName MuzzleAttachPointR;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MachineGunTracer;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MachineGunMuzzleFX;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* SniperMuzzleFX;

	UParticleSystemComponent* TracerParticleL;
	UParticleSystemComponent* TracerParticleR;
	UParticleSystemComponent* FlashParticleL;
	UParticleSystemComponent* FlashParticleR;

	int32 CurrentAmmo;

	int32 CurrentClip;

	FTimerHandle TimerHandle_HandleFiring;

	void SetOwningPawn(AMainPawn *NewOwner);

	void ChangeWeapon(int WeaponID);

	void ReloadAmmo();

	void fireTargetingRaycasts();

	void setInstantTarget();

	void setProjectileTarget();

	AActor* getTarget();
	
	UFUNCTION(Server, Reliable, WithValidation = Server_DealDamage_Validate)
		void Server_DealDamage(const FHitResult &Impact, const FVector& Shootdir, FWeaponData Weaponconfig);
	virtual void Server_DealDamage_Implementation(const FHitResult &Impact, const FVector& Shootdir, FWeaponData Weaponconfig);
	virtual bool Server_DealDamage_Validate(const FHitResult &Impact, const FVector& Shootdir, FWeaponData Weaponconfig);

	UFUNCTION()
	void OnRep_MyPawn();

	AActor* HitActor;
	AActor* Target;

	bool StartFiring;
	bool targeting;
	bool RightFired;

	float CrosshairRange;

	float LastFireTime;
	float GameTime;

protected:

	FHitResult WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo) const;

	FHitResult Up_hit;
	FHitResult Down_hit;
	FHitResult Left_hit;
	FHitResult Right_hit;

	FTimerHandle time_handler;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	AMainPawn* MyPawn;
};
