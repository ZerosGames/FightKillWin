// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "BaseWeapon.h"
#include "Main_Spectator_Pawn.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
*
*/
UCLASS()
class MYPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()


public:

	AMainPlayerController();

	UFUNCTION(Server, Reliable, WithValidation = ServerRespawn_Validate)
	void ServerRespawn();
	virtual void ServerRespawn_Implementation();
	virtual bool ServerRespawn_Validate();

	UFUNCTION(Server, Reliable, WithValidation = Server_Travel_Validate, BlueprintCallable, Category = MyPlayerController)
	void Server_Travel();
	virtual void Server_Travel_Implementation();
	virtual bool Server_Travel_Validate();

	UClass* GetPlayerPawnClass();

	void HandleRespawn(FVector DeathLocation);
	void RespawnTimer();
	void ToLobbyTimer();
	void NotifyWinState(FString Winner);
	void NotifyGameStart();
	void OutOfBounds();

	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void PostInitializeComponents() override;


	UFUNCTION(Reliable, Client, BlueprintCallable, Category = MyPlayerController)
	void DeterminePawnClass(int32 PawnIt);
	virtual void DeterminePawnClass_Implementation(int32 PawnIt);

	void OnChangePawn(TSubclassOf<APawn> _Pawn);

	int32 PawnSelection;

	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
	void SetPawnSelection(int32 Selection);

	void PauseGame();
	void RespawnTimerWidget();
	void GameEndWidget();
	

	UPROPERTY(Replicated)
	int32 TeamNum;
	UPROPERTY(Replicated)
	int32 Score;
	UPROPERTY(Replicated)
	bool Ready;

	////////////////////////////////////Getters/Setter///////////////////////////////////////////
	UFUNCTION(Reliable, Server, WithValidation = ServerSetReady_Validate, BlueprintCallable, Category = MyPlayerController)
	void ServerSetReady(bool _Ready);
	virtual void ServerSetReady_Implementation(bool _Ready);
	virtual bool ServerSetReady_Validate(bool _Ready);

	UFUNCTION(BlueprintCallable, Category = MyPlayerController)
	bool GetReady();

	void SetScore(int32 _score);
	int GetScore();

	UFUNCTION(Reliable, Server, WithValidation = ServerSetTeamNum_Validate, BlueprintCallable, Category = MyPlayerController)
	void ServerSetTeamNum(int32 _TeamNum);
	virtual void ServerSetTeamNum_Implementation(int32 _TeamNum);
	virtual bool ServerSetTeamNum_Validate(int32 _TeamNum);

	int GetTeamNum();

	UPROPERTY()
		UUserWidget* CurrentWidget;

	UPROPERTY()
		UUserWidget* CurrentHUDWidget;

protected:

	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerSetPawn(TSubclassOf<APawn> InPawnClass);
	virtual void ServerSetPawn_Implementation(TSubclassOf<APawn> InPawnClass);
	virtual bool ServerSetPawn_Validate(TSubclassOf<APawn> InPawnClass);

	FTimerHandle CountdownTimerHandle;
	FTimerHandle ToLobbyCountdownTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Repsawning")
	int32 RCountDownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ToLobby")
	int32 ToLobbyCountDownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Planes")
	TArray<TSubclassOf<APawn>> Planes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> InGameMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> RespawnTimerMenu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> EndGameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> StartGameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> OutOfBoundsWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
		TSubclassOf<UUserWidget> HUD;

	UPROPERTY()
		UUserWidget* CurrentStartGameWidget;

	UPROPERTY(Replicated)
		TSubclassOf<APawn> MyPawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpecPawn")
		TSubclassOf<AMain_Spectator_Pawn> Spectator_Pawn;

	bool GiveInput;
	AMainPawn* Pawn;
};
