// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MyGameState.h"


AMyGameState::AMyGameState()
{
	GameState = 0;
}

int AMyGameState::GetGameState()
{
	return GameState;
}

void AMyGameState::SetGameState(int _Gamestate)
{
	GameState = _Gamestate;
}