// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Lobby_Widget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ULobby_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = MyUUserWidgetClass)
	void InvertYAxis();

};
