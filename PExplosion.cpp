// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "PExplosion.h"


// Sets default values
APExplosion::APExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;
}

// Called when the game starts or when spawned
void APExplosion::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(1.0f);
}

// Called every frame
void APExplosion::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

