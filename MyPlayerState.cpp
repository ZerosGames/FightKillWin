// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MyPlayerState.h"

AMyPlayerState::AMyPlayerState()
{
	TeamNum = 0;
	Score = 0;
	Ready = false;
}

//////////////////////////////////////Getter/Setters///////////////////////////////////////////

void AMyPlayerState::SetReady(bool _Ready)
{
	Ready = _Ready;
}

bool AMyPlayerState::GetReady()
{
	return Ready;
}

void AMyPlayerState::SetTeamNum(int newTeamNum)
{
	TeamNum = newTeamNum;
}

int AMyPlayerState::GetTeamNum()
{
	return TeamNum;
}

void AMyPlayerState::SetScore(int _score)
{
	Score = _score;
}

int AMyPlayerState::GetScore()
{
	return Score;
}


