// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

UCLASS()
class MYPROJECT_API AMyGameState : public AGameState
{
	GENERATED_BODY()

public:

	AMyGameState();
	
	int GameState;

	void SetGameState(int);
	int GetGameState();
	
};
