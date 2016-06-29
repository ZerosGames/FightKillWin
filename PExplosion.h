// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PExplosion.generated.h"

UCLASS()
class MYPROJECT_API APExplosion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APExplosion();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(Category = Scene, VisibleDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* Scene;
};
