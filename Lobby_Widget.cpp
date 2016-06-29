// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "Lobby_Widget.h"

//called by the blueprint widget when server clicks the start game button.
void ULobby_Widget::InvertYAxis()
{
	AMainPlayerController* PC = Cast<AMainPlayerController>(GetOwningPlayerPawn()->Controller);

	PC->PlayerInput->InvertAxis("Pitch");
}


