// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MainPawn.generated.h"


UCLASS()
class MYPROJECT_API AMainPawn : public APawn
{
	GENERATED_BODY()

public:

	AMainPawn();

	////////////////////////////////////////////GamePlay//////////////////////////////

	void Start();
	void Pause();
	void UnPause();

	///////////////////////////////////////////Gameplay vars///////////////////////////

	UPROPERTY(Replicated)
	float Health;
	UPROPERTY(Replicated)
	AController* MyPc;
	float MaxHealth;
	bool InputAllowed;

	UPROPERTY(Replicated)
	bool hasMatchStarted;

	bool Accelerate;
	float Roll;
	bool Paused;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OutOfBounds")
	int32 CountOOBDownTimer;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* EngineParticle;

	AActor* TargetedActor;

	////////////////////////////////////////////Actor Sound////////////////////////////////
	

	////////////////////////////////////////////Actor Functions////////////////////////////

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	////////////////////////////////////////////Getters//////////////////////////////////////////////

	
	TArray<AMainPawn*> tempPawn;
	UPROPERTY(Replicated)
	TArray<AMainPawn*> EnemysTempPawn;
	UPROPERTY(Replicated)
	TArray<AMainPawn*> FriendlysTempPawn;

	TArray<AMainPawn*> GetFriendlysOverlapPawn();
	TArray<AMainPawn*> GetEnemyOverlapPawn();
	TArray<AMainPawn*> GetOverlapPawn();

	TArray<AActor*> tempActor;
	UPROPERTY(Replicated)
	TArray<AActor*> EnemysTempActor;
	UPROPERTY(Replicated)
	TArray<AActor*> FriendlysTempActor;

	TArray<AActor*> GetFriendlysOverlapActor();
	TArray<AActor*> GetEnemyOverlapActor();
	TArray<AActor*> GetOverlapActor();

	void TargetedByMissle(AActor* _Target);
	float GetAcceleration();
	float GetSpeed();
	AActor* GetTarget();
	ABaseWeapon* getWeapon();
	AController* GetPC();


	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }

	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }

	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

	FORCEINLINE class UArrowComponent* GetCrosshair() const { return Crosshair; }

private:

	/////////////////////////Weapon Functions/////////////////////////////////////////

	UFUNCTION()
	void OnRep_SpawnedWeapon(ABaseWeapon* LastWeapon);
	void SpawnWeapon();
	void StartTargeting();
	void StopTargeting();
	void EquipWeapon(ABaseWeapon* Weapon);
	void SetCurrentWeapon(ABaseWeapon* BaseWeapon);
	void StartFiring();
	void ChangeWeaponUp();
	void ChangeWeaponDown();
	void FireInputBegin();
	void FireInputEnd();
	void PlayHit(float Damage, struct FDamageEvent const& DamageEvent);

	UFUNCTION(reliable, server, WithValidation = ServerEquipWeapon_Validate)
	void ServerEquipWeapon(ABaseWeapon* Weapon);
	virtual void ServerEquipWeapon_Implementation(ABaseWeapon* Weapon);
	virtual bool ServerEquipWeapon_Validate(ABaseWeapon* Weapon);

	UFUNCTION(reliable, server, WithValidation = ServerExplosion_Validate)
	void ServerExplosion(AActor* _Explosion);
	virtual void ServerExplosion_Implementation(AActor* _Explosion);
	virtual bool ServerExplosion_Validate(AActor* _Explosion);

	//////////////////////////Weapon Vars/////////////////////////////////////////////

	TSubclassOf<class AActor> BP_Explosion;
	TSubclassOf<class AActor> BP_BaseWeapon;
	int WeaponID;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_SpawnedWeapon)
		ABaseWeapon* CurrentWeapon;
	

	UParticleSystemComponent* EngineLeft;
	UParticleSystemComponent* EngineRight;

	bool IsFiring;
	bool Targeting;
	bool Died;
	bool StartedRoll;

	////////////////////////////////PlayerMovement////////////////////////////////////////

	UFUNCTION(Client, Reliable)
		void ClientAddThrust(FVector Transform, float thrust);

	UFUNCTION(Client, Reliable)
		void ClientAddRotation(FRotator Rotation, FRotator MeshRotation, UStaticMeshComponent* _Mesh);

	UFUNCTION(Server, Reliable, WithValidation = ServerAddThrust_Validate)
		void ServerAddThrust(FVector Transform);
	virtual void ServerAddThrust_Implementation(FVector Transform);
	virtual bool ServerAddThrust_Validate(FVector Transform);

	UFUNCTION(Server, Reliable, WithValidation = ServerAddRotation_Validate)
		void ServerAddRotation(FRotator Rotation, FRotator MeshRotation, UStaticMeshComponent* _Mesh);
	virtual void ServerAddRotation_Implementation(FRotator Rotation, FRotator MeshRotation, UStaticMeshComponent* _Mesh);
	virtual bool ServerAddRotation_Validate(FRotator Rotation, FRotator MeshRotation, UStaticMeshComponent* _Mesh);

	void SetPosition();

	void AddAcceleration();
	void SubAcceleration();

	void ThrustInput();
	void PitchInput(float);
	void YawInput(float);
	void FlipCameraInputBegin();
	void FlipCameraInputEnd();

	void RollLeft();
	void RollRight();

	void RotateInput(float, float, float, float);

	/////////////////////////////////////////Player Movement vars///////////////////////////////////////////

	float MouseSensetivity;

	float Acceleration;
	float Speed;
	float MaxSpeed;
	float MinSpeed;

	float Pitch;
	float Yaw;
	float MeshRoll;
	float PawnRoll;

	float PitchTurnSpeed;
	float YawTurnSpeed;
	float RollTurnSpeed;

	float CurrentRollSpeed;
	float CurrentPitchSpeed;
	float CurrentYawSpeed;

	float StrifeYaw;
	float CurrentStrifeYaw;
	float StrifePitch;
	float CurrentStrifePitch;

	/////////////////////////////////////////Extended Movement//////////////////////////////////////////

	void ToggleRollRight();
	void ToggleRollLeft();
	void HandleExtendedMovement();
	void HandleRollCooldown();

	///////////////////////////////////////OutOfBounds/////////////////////////////////////////////////////////

	FTimerHandle CountOOBDownTimerHandle;
	void OutOfBoundsTimer();
	bool OutOfBounds;


	///////////////////////////////////////Extended Movement vars/////////////////////////////////////////

	bool bIsOnCooldown;
	bool bIsRolling;
	bool bRollStarted;
	float RollingSign;
	FRotator CurrentRotation;
	FRotator TargetRotation;
	float RollingRotationStep;
	float RollingPositionStep;
	FTimerHandle RollTimerHandle;
	int RollTimer;

	////////////////////////////////////////////////////////Gameplay Privates/////////////////////////////////////

	void Die();

protected:

	///////////////////////////////////////////////////Actor Setup//////////////////////////////////////////

	UPROPERTY(Category = Scene, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class USceneComponent * Scene;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, Replicated, meta = (AllowPrivateAcess = "true"))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(Category = Collision, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class UBoxComponent* BoxCollidier;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(Category = Movement, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UFloatingPawnMovement* MovementComponent;

	UPROPERTY(Category = Collision, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionSphere;

	UPROPERTY(Category = SightSphere, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SightSphere;

	UPROPERTY(Category = TargetSphere, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TargetSphere;

	UPROPERTY(Category = Crosshair, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* Crosshair;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* GunSounds;

	//////////////////////////////////////////////////////////Collision////////////////////////////////////////////////////////

	UFUNCTION()
	void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnBoxHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
