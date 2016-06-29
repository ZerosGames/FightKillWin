// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MainPlayerController.h"
#include "MainPawn.h"
#include "MyPlayerState.h"
#include "Engine.h"
#include "MainHUD.h"
#include "math.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"


AMainPlayerController::AMainPlayerController()
{
	RCountDownTimer = 10;
	ToLobbyCountDownTimer = 20;
	PawnSelection = 1;
	Ready = false;
	GiveInput = false;
	TeamNum = 1;
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MyPawnClass = Planes[0];
	DeterminePawnClass(0);

	if (HUD)
	{
		CurrentHUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUD);
	}

	if (CurrentHUDWidget != nullptr)
	{
		CurrentHUDWidget->AddToViewport();
	}

}

void AMainPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyPawnClass = Planes[0];
}

void AMainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (HUD && CurrentHUDWidget == nullptr)
	{
		CurrentHUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUD);
	}

	if (CurrentHUDWidget != nullptr && !CurrentHUDWidget->IsInViewport())
	{
		CurrentHUDWidget->AddToViewport();
	}
	
}

void AMainPlayerController::ServerRespawn_Implementation()
{
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

bool AMainPlayerController::ServerRespawn_Validate()
{
	return true;
}

void AMainPlayerController::Server_Travel_Implementation()
{
	GetWorld()->ServerTravel("/Game/Levels/Level_Starter");
}

bool AMainPlayerController::Server_Travel_Validate()
{
	return true;
}

void AMainPlayerController::RespawnTimer()
{
	RCountDownTimer--;
	if (RCountDownTimer < 1)
	{
		if (GetPawn())
		{
			AMain_Spectator_Pawn* TempSpecPawn = Cast<AMain_Spectator_Pawn>(GetPawn());
			if (TempSpecPawn)
			{
				TempSpecPawn->DetachFromControllerPendingDestroy();
				TempSpecPawn->Destroy();
			}
		}
		RespawnTimerWidget();
		ServerRespawn();
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		RCountDownTimer = 10;
	}
}

void AMainPlayerController::HandleRespawn(FVector DeathLocation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;

	FVector SpecLoc = DeathLocation;
	FRotator SpecRot = FRotator(-90,0,0);

	AMain_Spectator_Pawn* SpecPawn = GetWorld()->SpawnActor<AMain_Spectator_Pawn>(Spectator_Pawn, SpecLoc, SpecRot,SpawnParams);

	Possess(SpecPawn);

	RespawnTimerWidget();
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AMainPlayerController::RespawnTimer, 1.0f, true);
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &AMainPlayerController::PauseGame);
	InputComponent->BindAction("Test", IE_Pressed, this, &AMainPlayerController::ServerRespawn);
}

void AMainPlayerController::PauseGame()
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	else
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), InGameMenuClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AMainPlayerController::RespawnTimerWidget()
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	else
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), RespawnTimerMenu);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AMainPlayerController::DeterminePawnClass_Implementation(int32 PawnIt)
{
	if (IsLocalController()) //Only Do This Locally (NOT Client-Only, since Server wants this too!)
	{
		ServerSetPawn(Planes[PawnIt]);
		return;
	}
}

bool AMainPlayerController::ServerSetPawn_Validate(TSubclassOf<APawn> InPawnClass)
{
	return true;
}

void AMainPlayerController::ServerSetPawn_Implementation(TSubclassOf<APawn> InPawnClass)
{
	MyPawnClass = InPawnClass;
	//ServerRespawn();
}

// Replication
void AMainPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AMainPlayerController, MyPawnClass);
	DOREPLIFETIME(AMainPlayerController, TeamNum);
	DOREPLIFETIME(AMainPlayerController, Ready);
}

UClass* AMainPlayerController::GetPlayerPawnClass()
{
	return MyPawnClass;
}

void AMainPlayerController::SetPawnSelection(int32 Selection)
{
	PawnSelection = Selection;
}

void AMainPlayerController::ServerSetReady_Implementation(bool _Ready)
{
	Ready = _Ready;
}

bool AMainPlayerController::ServerSetReady_Validate(bool _Ready)
{
	return true;
}

bool AMainPlayerController::GetReady()
{
	return Ready;
}

void AMainPlayerController::ServerSetTeamNum_Implementation(int newTeamNum)
{
	TeamNum = newTeamNum;
}

bool AMainPlayerController::ServerSetTeamNum_Validate(int newTeamNum)
{
	return true;
}

int AMainPlayerController::GetTeamNum()
{
	return TeamNum;
}

void AMainPlayerController::SetScore(int32 _score)
{
	Score = _score;
}

int AMainPlayerController::GetScore()
{
	return Score;
}

void AMainPlayerController::NotifyWinState(FString Winner)
{
	if (GetPawn())
	{
		AMainPawn* Pawn = Cast<AMainPawn>(GetPawn());
		if (Pawn)
		{
			Pawn->DetachFromControllerPendingDestroy();
			Pawn->Destroy();
		}
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;

	FVector SpecLoc = FVector(0, 0, 4000);
	FRotator SpecRot = FRotator(-90, 0, 0);

	AMain_Spectator_Pawn* SpecPawn = GetWorld()->SpawnActor<AMain_Spectator_Pawn>(Spectator_Pawn, SpecLoc, SpecRot, SpawnParams);

	Possess(SpecPawn);

	if (Winner == "Blue")
	{
		if (Role == ROLE_Authority)
		{
			GameEndWidget();
			GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AMainPlayerController::ToLobbyTimer, 1.0f, true);
		}
	}
	else if (Winner == "Red")
	{
		if (Role == ROLE_Authority)
		{
			GameEndWidget();
			GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AMainPlayerController::ToLobbyTimer, 1.0f, true);
		}
	}
}

void AMainPlayerController::ToLobbyTimer()
{
	ToLobbyCountDownTimer--;

	if (ToLobbyCountDownTimer < 1)
	{
		GameEndWidget();

		if (Role == ROLE_Authority)
		{
			GetWorld()->ServerTravel("/Game/Levels/Level_Lobby");
		}
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		ToLobbyCountDownTimer = 20;
	}
}

void AMainPlayerController::GameEndWidget()
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	else
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), EndGameMenu);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AMainPlayerController::OutOfBounds()
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	else
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), OutOfBoundsWidget);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
