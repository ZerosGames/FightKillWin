// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "Main_Base.h"
#include "MyProjectGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMyProjectGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	AMyProjectGameMode();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void PostInitializeComponents() override;

	void HandleTeamPlacement(APlayerController* PC);
	void SpawnBases();
	void NotifyGameEnd(AMain_Base* Winner);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;


	/////// Variables for Blueprints ///////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		int32 BoxSize;

	/////// Variables ///////

	int NumberOfControllers;
	TArray<FVector> Waypoints;

	TArray<APlayerController*> Players;

	FTimerHandle GameStartTimerHandle;

	int MaxPlayersPerTeam;
	int BlueTeamNum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WinningTeam")
	FString TeamWinner;
	int RedTeamNum;
	bool TeamAssignment;

	TArray<APlayerStart*> PlayerBlueSpawnPoint;
	TArray<APlayerStart*> AIBlueSpawnPoint;
	TArray<APlayerStart*> PlayerRedSpawnPoint;
	TArray<APlayerStart*> AIRedSpawnPoint;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bases")
	TArray<TSubclassOf<AMain_Base>> Bases;

protected:

	AMain_Base* BlueBase;
	AMain_Base* RedBase;
	TArray<AMain_Base*> SpawnedBases;
	bool GameHasStarted;
};
