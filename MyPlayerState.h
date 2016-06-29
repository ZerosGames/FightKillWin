// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

UCLASS()
class MYPROJECT_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AMyPlayerState();

	////////////////////////////////////Public vars//////////////////////////////////////////////
	int32 TeamNum;
	int Score;
	bool Ready;
	
	////////////////////////////////////Getters/Setter///////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = MyPlayerState)
	void SetReady(bool _Ready);
	UFUNCTION(BlueprintCallable, Category = MyPlayerState)
	bool GetReady();

	void SetScore(int);
	int GetScore();

	UFUNCTION(BlueprintCallable, Category = MyPlayerState)
	void SetTeamNum(int32 _TeamNum);
	int GetTeamNum();
};
