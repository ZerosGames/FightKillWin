// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MyProjectGameMode.h"
#include "MainPawn.h"
#include "Main_base.h"
#include "MainPlayerController.h"
#include "MainHUD.h"
#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	bUseSeamlessTravel = true;

	PlayerControllerClass = AMainPlayerController::StaticClass();
	DefaultPawnClass = AMainPawn::StaticClass();

	HUDClass = AMainHUD::StaticClass();

	MaxPlayersPerTeam = 5;
	TeamWinner = 0;
	TeamAssignment = false;

	bUseSeamlessTravel = true;
	GameHasStarted = false;
}

void AMyProjectGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AMyProjectGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* TestSpawn = *It;

		if (TestSpawn->PlayerStartTag == "BluePlayer")
		{
			PlayerBlueSpawnPoint.Add(TestSpawn);
		}
		else if (TestSpawn->PlayerStartTag == "RedPlayer")
		{
			PlayerRedSpawnPoint.Add(TestSpawn);
		}
		else if (TestSpawn->PlayerStartTag == "BlueAI")
		{
			AIBlueSpawnPoint.Add(TestSpawn);
		}
		else if (TestSpawn->PlayerStartTag == "RedAI")
		{
			AIRedSpawnPoint.Add(TestSpawn);
		}
	}
}

void AMyProjectGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnBases();
}

void AMyProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	Players.Add(NewPlayer);

	AMainPlayerController* PC = Cast<AMainPlayerController>(NewPlayer);

	if (PC != nullptr)
	{
		if (PC->GetTeamNum() == 0)
		{
			HandleTeamPlacement(NewPlayer);
			
		}
		PC->ServerRespawn();
	}
}

AActor* AMyProjectGameMode::ChoosePlayerStart_Implementation(AController* Player)
{

	AMainPlayerController* PC = Cast<AMainPlayerController>(Player);
	APlayerStart* PlayerStart = NULL;

	if (PC != nullptr)
	{
		if (PC->GetTeamNum() == 1)
		{
			PC->Tags.Add("BLUE");
			for (APlayerStart* temp : PlayerBlueSpawnPoint)
			{
				if (!temp->ActorHasTag("Taken"))
				{
					temp->Tags.Add("Taken");
					PlayerStart = temp;
					break;
				}
			}
		}
		else if (PC->GetTeamNum() == 2)
		{
			PC->Tags.Add("RED");
			for (APlayerStart* temp : PlayerRedSpawnPoint)
			{
				if (!temp->ActorHasTag("Taken"))
				{
					temp->Tags.Add("Taken");
					PlayerStart = temp;
					break;
				}
			}
		}
	}

	return PlayerStart ? PlayerStart : Super::ChoosePlayerStart_Implementation(Player);
}

void AMyProjectGameMode::HandleTeamPlacement(APlayerController* Player)
{
	AMainPlayerController* CastedPlayer = Cast<AMainPlayerController>(Player);

	if (CastedPlayer != nullptr)
	{
		if (BlueTeamNum < MaxPlayersPerTeam)
		{
			CastedPlayer->ServerSetTeamNum(1);
			BlueTeamNum++;
			TeamAssignment = true;
			CastedPlayer->ServerRespawn();
		}
		else if (RedTeamNum < MaxPlayersPerTeam)
		{
			CastedPlayer->ServerSetTeamNum(2);
			RedTeamNum++;
			TeamAssignment = false;
			CastedPlayer->ServerRespawn();
		}
	}
}

UClass* AMyProjectGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AMainPlayerController* PC = Cast<AMainPlayerController>(InController);

	if (PC)
	{
		return PC->GetPlayerPawnClass();
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AMyProjectGameMode::SpawnBases()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;

	FVector Red_BaseLoc = FVector(1670.0f, -13200.0f, 300.0f);
	FRotator Red_BaseRot = FRotator::ZeroRotator;
	FVector Blue_BaseLoc = FVector(-1670.0f, 13200.0f, 300.0f);
	FRotator Blue_BaseRot = FRotator::ZeroRotator;

	BlueBase = GetWorld()->SpawnActor<AMain_Base>(Bases[0], Blue_BaseLoc, Blue_BaseRot, SpawnParams);
	RedBase = GetWorld()->SpawnActor<AMain_Base>(Bases[1], Red_BaseLoc, Red_BaseRot, SpawnParams);
}

void AMyProjectGameMode::NotifyGameEnd(AMain_Base* Winner)
{
	if (Winner->GetName().Contains("Blue"))
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), Iterator.GetIndex()));
			if (PC)
			{
				TeamWinner = " Red";
				PC->NotifyWinState("Red");
				
			}
		}
	}
	else if(Winner->GetName().Contains("Red"))
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), Iterator.GetIndex()));
			if (PC)
			{
				TeamWinner = " Blue";
				PC->NotifyWinState("Blue");
			}
		}
	}
}
