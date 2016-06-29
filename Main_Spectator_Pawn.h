// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Main_Spectator_Pawn.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMain_Spectator_Pawn : public APawn
{
	GENERATED_BODY()

	AMain_Spectator_Pawn();

public:

	UPROPERTY(Category = Scene, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class USceneComponent * Scene;
	
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class UCameraComponent* Camera;

	virtual void Tick(float DeltaSeconds) override;

	float RotSpeed;
};
