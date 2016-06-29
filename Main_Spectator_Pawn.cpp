// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "Main_Spectator_Pawn.h"

AMain_Spectator_Pawn::AMain_Spectator_Pawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->SetRelativeRotation(FRotator(0, -10, 0));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);

	RotSpeed = 0;
}

void AMain_Spectator_Pawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RotSpeed += 0.3f;
	SetActorRotation(FRotator(-20, RotSpeed, 0));
}